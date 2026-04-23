/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Response.cpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: pmorello <pmorello@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/04/16 10:37:25 by pmorello          #+#    #+#             */
/*   Updated: 2026/04/16 13:39:48 by pmorello         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "web_server.hpp"

Response::Response(const Request &req) : _request(req)
{
    _full_path = "";    
    _body.clear();
    _redirectCode = 0;
    _redirectUrl = "";
    _cgiFlag = 0;
    _contentLength = 0;
    _autoindex = false;
}

Response::Response(const Response &src) : _request(src._request)
{
    *this = src; 
}

Response    &Response::operator=(const Response &src)
{
    if (this != &src)
    {
        _full_path = src._full_path;
        _body = src._body;
        _redirectCode = src._redirectCode;
        _redirectUrl = src._redirectUrl;
        _cgiFlag = src._cgiFlag;
        _contentLength = src._contentLength;
        _autoindex = src._autoindex;
        _serverConfig = src._serverConfig;
        _location = src._location;
    }
    return (*this);
}

Response::~Response(){}

void    Response::setServer(const Server *serv)
{
    this->_server = serv;
}

void    Response::setServerConfig(const ServerConfig *servConf)
{
    this->_serverConfig = servConf;
}

int Response::buildPath()
{
    if (!_serverConfig)
    {
        std::cout << "Error: No hi ha configuracio carregada a la Response " << std::endl;
        _redirectCode = 500;
        return (1);
    }
    std::string _urlMatch = "";
    findURL(_request.getPath(), _serverConfig->getLocations(), _urlMatch);
    if (_urlMatch.empty()) 
    {
        _redirectCode = 404;
        return (1);
    }
    const LocationConfig *urlAcces = NULL;
    const std::vector<LocationConfig> &locs = _serverConfig->getLocations();
    for (size_t i = 0; i < locs.size(); i++) 
    {
        if (locs[i].getPath() == _urlMatch) 
        {
            urlAcces = &locs[i];
            _location = locs[i];
            break;
        }
    }
    if (!urlAcces) 
    {
        _redirectCode = 404;
        return (1);
    }
    if (validMethods(_request.getMethods(), urlAcces,_redirectCode)) 
    {
        std::cout << "NOT ALLOWED METHOD " << std::endl;
        return (1);   
    }
    if (_request.getBodyString().length() > _serverConfig->getClientMaxBodySize()) 
    {
        _redirectCode = 413;
        return (1);
    }
    if (urlAcces->getRedirectCode() != 0) 
    {
        _redirectCode = urlAcces->getRedirectCode();
        _redirectUrl = urlAcces->getRedirectUrl();
        return (1);
    }
    if (urlAcces->getPath().find("cgi-bin") != std::string::npos) 
    {
        _cgiFlag = 1;
        // return (logica de la preparacio del CGI);
    }
    _full_path = urlAcces->getRoot() + _request.getPath();
    if (!urlAcces->getCgiExtension().empty()) 
    {
        std::string ext = urlAcces->getCgiExtension();
        if (_full_path.length() >= ext.length() && 
            _full_path.compare(_full_path.length() - ext.length(), ext.length(), ext) == 0) {
            _cgiFlag = 1;
            // return (logica de la preparacio del CGI);
        }
    }
    if (isDirectory(_full_path)) 
    {
        if (_full_path.empty() || _full_path[_full_path.length() - 1] != '/') 
        {
            _redirectCode = 301;
            _redirectUrl = _request.getPath() + "/";
            return (1);
        }
        const std::vector<std::string> &indexFile = urlAcces->getIndexFiles();
        std::string foundIndex = "";
        for (size_t i = 0; i < indexFile.size(); i++)
        {
            std::string fullIndexPath = _full_path + indexFile[i];
            if (fileExist(fullIndexPath))
            {
                foundIndex = indexFile[i];
                _full_path = fullIndexPath;
                break ;
            }
        }
        if (!indexFile.empty())
        {
            if (urlAcces->getAutoindex()) 
            {
                _autoindex = true;
                return (0);
            } 
            else 
            {
                _redirectCode = 403;
                return (1);
            }
        }
    }
    if (!fileExist(_full_path)) 
    {
        _redirectCode = 404;
        return (1);
    }
    return (0);
}

int Response::readFile()
{
    std::ifstream file(_full_path.c_str(), std::ios::binary | std::ios::ate);
    if (!file) 
    {
        _redirectCode = 404;
        return (1);
    }   
    std::streamsize size = file.tellg();
    file.seekg(0, std::ios::beg);
    _body.resize(size);
    if (!file.read(reinterpret_cast<char*>(&_body[0]), size)) 
    {
        _redirectCode = 500;
        return (1);
    }
    _contentLength = _body.size();
    return (0);
}

int Response::buildBody()
{
    try 
    {
        if (buildPath())
        {
            buildErrorBody();
            return (1);
        }
        if (_cgiFlag || _autoindex)
            return (0);
        if (_redirectCode != 0 && _redirectCode != 200)
            return (0);
        std::string method_str = methodToStr(_request.getMethods());
        if (method_str == "GET")
        {
            if (readFile())
            {
                _redirectCode = 404;
                buildErrorBody();
                return (1);
            }
        }
        else if (method_str == "POST" || method_str == "PUT") 
        {
            std::ofstream file(_full_path.c_str(), std::ios::binary);
            if (file.fail()) 
            {
                _redirectCode = 500;
                buildErrorBody();
                return (1);
            }
            if (_request.getMultiformFlag()) 
            {
                std::vector<uint8_t> cleanData = removeBoundary(_request.getBodyBits(), _request.getBoundary());
                file.write(reinterpret_cast<const char*>(&cleanData[0]), cleanData.size());
            }
            else
            {
                const std::vector<uint8_t>& rawBody = _request.getBodyBits();
                if (!rawBody.empty())
                    file.write(reinterpret_cast<const char*>(&rawBody[0]), rawBody.size());
            }
            file.close();
            if (fileExist(_full_path))
                _redirectCode = 200;
            else
                _redirectCode = 201;
        }
        _contentLength = _body.size();
        if (_redirectCode == 0)
            _redirectCode = 200;
    }
    catch (const Response::BodyEmptyException &e) 
    {

        _redirectCode = 400;
        buildErrorBody();
        return (1);
    }
    return (0);
}

void Response::setHeaders() {
    _contentResponse = "HTTP/1.1 " + codeToStr(_redirectCode) + " " + statusCodeToString(_redirectCode) + "\r\n";   
    _contentResponse += "Server: Webserv/1.0 (Unix)\r\n";
    char buf[100];
    time_t now = time(0);
    struct tm tm = *gmtime(&now);
    strftime(buf, sizeof(buf), "%a, %d %b %Y %H:%M:%S %Z", &tm);
    _contentResponse += "Date: " + std::string(buf) + "\r\n";
    _contentType = _mime.getMimeType(_full_path);
    _contentResponse += "Content-Type: " + _contentType + "\r\n";
    std::stringstream ss;
    ss << _contentLength;
    _contentResponse += "Content-Length: " + ss.str() + "\r\n";
    _contentResponse += "Connection: keep-alive\r\n\r\n"; 
}

void    Response::buildErrorBody()
{
    if (_serverConfig && _serverConfig->getErrorPages().count(_redirectCode))
    {
        _full_path = _location.getRoot() + _serverConfig->getErrorPages().at(_redirectCode);
        if (readFile())
            return ;
    }

}

int Response::buildHtmlIndex()
{
    struct dirent   *entityStruct;
    DIR             *directory;
    std::string     dirListPage;
    
    directory = opendir(_full_path.c_str());
    if (directory == NULL)
    {    
        return (1);
    }
    dirListPage.append("<html>\n<head>\n<title>Index of " + _full_path + "</title>\n");
    dirListPage.append("<style>body{font-family:sans-serif;} table{width:80%; margin:20px auto;} th{text-align:left; border-bottom:1px solid #ccc;}</style>\n");
    dirListPage.append("</head>\n<body>\n");
    dirListPage.append("<h1>Index of " + _request.getPath() + "</h1>\n<hr>\n");
    dirListPage.append("<table>\n<tr><th>File Name</th><th>Last Modification</th><th>File Size</th></tr>\n");
    struct stat file_stat;
    std::string file_path;
    while((entityStruct = readdir(directory)) != NULL)
    {
        if(std::string(entityStruct->d_name) == ".")
            continue;
        file_path = _full_path;
        if (file_path[file_path.length() - 1] != '/')
            file_path += "/";
        file_path += entityStruct->d_name;
        if (stat(file_path.c_str() , &file_stat) == -1)
            continue;
        dirListPage.append("<tr>\n<td>\n");
        dirListPage.append("<a href=\"");
        dirListPage.append(entityStruct->d_name);
        if (S_ISDIR(file_stat.st_mode))
            dirListPage.append("/");
        dirListPage.append("\">");
        dirListPage.append(entityStruct->d_name);
        if (S_ISDIR(file_stat.st_mode))
            dirListPage.append("/");
        dirListPage.append("</a>\n</td>\n<td>\n");
        dirListPage.append(ctime(&file_stat.st_mtime));
        dirListPage.append("</td>\n<td>\n");
        if (!S_ISDIR(file_stat.st_mode))
        {
            std::stringstream ss;
            ss << file_stat.st_size;
            dirListPage.append(ss.str() + " bytes");
        }
        else
            dirListPage.append("-");
        dirListPage.append("</td>\n</tr>\n");
    }
    dirListPage.append("</table>\n<hr>\n</body>\n</html>\n");
    closedir(directory);
    _body.clear();
    _body.insert(_body.begin(), dirListPage.begin(), dirListPage.end());
    _contentLength = _body.size();
    _contentType = "text/html"; 
    return (0);
}

void Response::buildResponse()
{
    if (buildBody())
        buildErrorBody();
    if (_cgiFlag)
        // std::string CGires = cgiObject.getOutput();
        // _contentResponse = cgiRes; 
    if (_autoindex)
    {
        if (buildHtmlIndex())
        {
            _redirectCode = 500;
            buildErrorBody();
        }
        else
            _redirectCode = 200;
    }
    setHeaders();
    if ((_request.getMethods() == GET || _redirectCode != 200))
        _contentResponse.append(_body.begin(), _body.end());
}  


void    Response::cutResponse(size_t i)
{
    _contentResponse = _contentResponse.substr(i);
}
void    Response::clear()
{
    _full_path.clear();
    _body.clear();
    _contentResponse.clear();
    _redirectCode = 0;
    _cgiFlag = 0;
    _autoindex = false;
    _contentLength = 0;
}

std::string Response::getResponse() const
{
    return (this->_contentResponse);
}

short   Response::getRedirectCode()
{
    return (this->_redirectCode);
}

size_t  Response::getContentLength()
{
    return (this->_contentResponse.length());
}

