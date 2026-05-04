/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ResponseBuilder.cpp                                :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: marvin <marvin@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/05/04 15:14:30 by marvin            #+#    #+#             */
/*   Updated: 2026/05/04 15:14:30 by marvin           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ResponseBuilder.hpp"

Response::Response(const HTTPRequest &req, HTTPResponse &res) : 
    _full_path(""), 
    _redirectUrl(""), 
    _cgiFlag(0), 
    _contentLength(0), 
    _autoindex(false),
    _contentType("text/plain"),
    _location(),
    _request(req),
    _serverConfig(NULL),
    _response(res)
{}


Response::~Response() {}

//setters
void Response::setMime(const MimeTypes& mime)
{ 
    this->_mime = mime; 
}

void Response::setLocation(const LocationConfig& loc) 
{ 
    this->_location = loc; 
}

void Response::setServerConfig(const ServerConfig* conf) 
{ 
    this->_serverConfig = conf; 
}


//getters
int         Response::getCgiFlag() const
{
    return (this->_cgiFlag);
}
std::string Response::getFullPath() const
{
    return (this->_full_path);
}


//utils
static void findURL(const std::string &urlClient, const std::vector<LocationConfig> &urlLocs, std::string &urlWinner)
{
    size_t  longestMatch = 0;
    for (std::vector<LocationConfig>::const_iterator it = urlLocs.begin(); it != urlLocs.end(); it++)
    {
        if (urlClient.compare(0, it->getPath().length(), it->getPath()) == 0) {
            if (it->getPath() == "/" || urlClient.length() == it->getPath().length() || urlClient[it->getPath().length()] == '/') {
                if (it->getPath().length() > longestMatch) {
                    longestMatch = it->getPath().length();
                    urlWinner = it->getPath();
                }
            }
        }
    }
}

static int  validMethods(const std::string  &methods, const LocationConfig *local, short &code)
{
    const std::vector<std::string> &method = local->getAllowedMethods();
    if (method.empty())
        return (0);
    for (std::vector<std::string>::const_iterator it = method.begin(); it != method.end(); it++) 
    {
        if (*it == methods) 
        {
            return (0);
        }
    }
    code = 405;
    return (1);
}

static bool isDirectory(std::string &_urlDirect)
{
    struct stat s;
    if (stat(_urlDirect.c_str(), &s) == 0)
        return (S_ISDIR(s.st_mode));
    return (false);
}

static bool fileExist(std::string &_urlFile)
{
    struct stat s;
    return (stat(_urlFile.c_str(), &s) == 0);
}

static std::string extractBoundary(const std::string &contentType)
{
    std::string key = "boundary";
    size_t pos = contentType.find(key);
    if (pos != std::string::npos)
        return (contentType.substr(pos + key.length()));
    return ("");
}

//logica
int Response::buildPath()
{
    if (!_serverConfig)
    {
        _response.setStatusCode(500);
        return (1);
    }
    std::string _urlMatch = "";
    findURL(_request.getPath(), _serverConfig->getLocations(), _urlMatch);
    if (_urlMatch.empty()) 
    {
        _response.setStatusCode(404);
        return (1);
    }
    const std::vector<LocationConfig> &locs = _serverConfig->getLocations();
    const LocationConfig *urlAcces = NULL;
    for (size_t i = 0; i < locs.size(); i++) 
    {
        if (locs[i].getPath() == _urlMatch) 
        {
            _location = locs[i];
            urlAcces = &_location;
            break;
        }
    }
    if (!urlAcces) 
    {
        _response.setStatusCode(404);
        return (1);
    }
    short currentCode = _response.getStatusCode();
    if (validMethods(_request.getMethod(), urlAcces, currentCode)) 
    {
        _response.setStatusCode(currentCode);
        return (1);   
    }

    if (_request.getBody().size() > _serverConfig->getClientMaxBodySize()) 
    {
        _response.setStatusCode(413);
        return (1);
    }
    if (urlAcces->getRedirectCode() != 0) 
    {
        _response.setStatusCode(urlAcces->getRedirectCode());
        _redirectUrl = urlAcces->getRedirectUrl();
        return (1);
    }
    if (urlAcces->getPath().find("cgi-bin") != std::string::npos || !urlAcces->getCgiExtension().empty()) 
    {
        _cgiFlag = 1;
        return (0);
    }
    _full_path = urlAcces->getRoot() + _request.getPath();
    if (isDirectory(_full_path)) 
    {
        if (_full_path[_full_path.length() - 1] != '/') 
        {
            _response.setStatusCode(301);
            _redirectUrl = _request.getPath() + "/";
            return (1);
        }
        const std::vector<std::string> &indexFiles = urlAcces->getIndexFiles();
        for (size_t i = 0; i < indexFiles.size(); i++)
        {
            std::string fullIndexPath = _full_path + indexFiles[i];
            if (fileExist(fullIndexPath))
            {
                _full_path = fullIndexPath;
                return (0);
            }
        }
        if (urlAcces->getAutoindex()) 
        {
            _autoindex = true;
            return (0);
        } 
        _response.setStatusCode(403);
        return (1);
    }

    if (!fileExist(_full_path)) 
    {
        _response.setStatusCode(404);
        return (1);
    }
    return (0);
}

int Response::readFile()
{
    std::ifstream file(_full_path.c_str(), std::ios::binary);
    if (!file) 
    {
        _response.setStatusCode(404);
        return (1);
    }   
    
    std::stringstream ss;
    ss << file.rdbuf();
    _response.setBody(ss.str());
    return (0);
}

int Response::buildBody()
{
    if (buildPath())
    {
        buildErrorBody();
        return (1);
    }
    if (_cgiFlag || _autoindex)
        return (0);
    std::string method = _request.getMethod();
    if (method == "GET")
    {
        if (readFile())
        {
            buildErrorBody();
            return (1);
        }
    }
    else if (method == "POST" || method == "PUT") 
    {
        std::ofstream file(_full_path.c_str(), std::ios::binary);
        if (!file.is_open()) 
        {
            _response.setStatusCode(500);
            buildErrorBody();
            return (1);
        }
        const std::vector<uint8_t>& rawBody = _request.getBody();
        file.write(reinterpret_cast<const char*>(rawBody.data()), rawBody.size());
        file.close();
        
        _response.setStatusCode(201); // Created
    }
    else if (method == "DELETE")
    {
        if (std::remove(_full_path.c_str()) != 0)
            _response.setStatusCode(404);
        else
            _response.setStatusCode(204); // No Content
    }
    if (_response.getStatusCode() == 0)
        _response.setStatusCode(200);
    return (0);
}

void Response::buildErrorBody()
{
    int code = _response.getStatusCode();
    if (_serverConfig && _serverConfig->getErrorPages().count(code))
    {
        _full_path = _location.getRoot() + _serverConfig->getErrorPages().at(code);
        if (readFile() == 0)
            return ;
    }
    _response = HTTPResponse::buildErrorResponse(code);
}

int Response::buildHtmlIndex()
{
    DIR *directory = opendir(_full_path.c_str());
    if (directory == NULL) 
        return (1);
    std::ostringstream oss;
    oss << "<html><head><title>Index of " << _request.getPath() << "</title></head>";
    oss << "<body><h1>Index of " << _request.getPath() << "</h1><hr><table>";
    struct dirent *entity;
    while((entity = readdir(directory)) != NULL)
    {
        if(std::string(entity->d_name) == ".") continue;
        oss << "<tr><td><a href=\"" << entity->d_name << "\">" 
            << entity->d_name << "</a></td></tr>";
    }
    oss << "</table><hr></body></html>";
    closedir(directory);
    _response.setBody(oss.str());
    _contentType = "text/html"; 
    return (0);
}

void Response::buildResponse()
{
    if (buildBody())
        return;
    if (_cgiFlag)
        return;
    if (_autoindex)
    {
        if (buildHtmlIndex())
        {
            _response.setStatusCode(500);
            buildErrorBody();
        }
        else
            _response.setStatusCode(200);
    }
    setHeaders();
}  

void Response::setHeaders()
{
    if (_contentType == "text/plain" && !_full_path.empty())
    {
        size_t dot_pos = _full_path.find_last_of(".");
        if (dot_pos != std::string::npos)
            _contentType = _mime.getMimeType(_full_path.substr(dot_pos));
    }
    _response.setHeader("Content-Type", _contentType);
    if (_request.getHeader("connection") == "keep-alive")
        _response.setHeader("Connection", "keep-alive");
    else
        _response.setHeader("Connection", "close");
    int code = _response.getStatusCode();
    if (code >= 300 && code < 400 && !_redirectUrl.empty())
        _response.setHeader("Location", _redirectUrl);
}