#include "ResponseBuilder.hpp"

Response::Response(const HTTPRequest &req, const HTTPResponse &res) : 
    _full_path(""), 
    _body(),              // Inicialitza un vector buit 
    _redirectUrl(""), 
    _cgiFlag(0), 
    _contentLength(0), 
    _autoindex(false),
    _contentType("text/plain"), // Valor per defecte segur
    _contentResponse(""),
    _location(),
    _request(req),      // Crida al constructor per defecte de LocationConfig
    _server(NULL),
    _serverConfig(NULL),
    _response(res)
{}

Response::Response(const Response &other) : 
    _full_path(other._full_path), 
    _body(other._body), 
    _redirectUrl(other._redirectUrl), 
    _cgiFlag(other._cgiFlag), 
    _contentLength(other._contentLength),
    _autoindex(other._autoindex),
    _contentResponse(""),
    _location(other._location),
    _request(other._request),      // Crida al constructor per defecte de LocationConfig
    _server(NULL),
    _serverConfig(NULL),
    _response(other._response)
{}

Response    &Response::operator=(const Response &other)
{
    if (this != &other)
    {
        _full_path = other._full_path;
        _body = other._body;
        _redirectUrl = other._redirectUrl;
        _cgiFlag = other._cgiFlag;
        _contentLength = other._contentLength;
        _autoindex = other._autoindex;
        _serverConfig = other._serverConfig;
        _location = other._location;
        _server = other._server;
        _serverConfig = other._serverConfig;
    }
    return (*this);
}

Response::~Response()
{

}

void    Response::setMime(const MimeTypes& mime)
{
    this->_mime = mime;
}

void    Response::setLocation(const LocationConfig& loc)
{
    this->_location = loc;
}

void    Response::setServer(const Server* serv)
{
    this->_server = serv;
}

void    Response::setServerConfig(const ServerConfig* conf)
{
    this->_serverConfig = conf;
}

int Response::buildPath(HTTPResponse &res)
{
    if (!_serverConfig)
    {
        res.setStatusCode(500);
        return (1);
    }
    std::string _urlMatch = "";
    findURL(_request.getPath(), _serverConfig->getLocations(), _urlMatch);
    if (_urlMatch.empty()) 
    {
        res.setStatusCode(404);
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
        res.setStatusCode(404);
        return (1);
    }
    short statusCode = res.getStatusCode();
    if (validMethods(_request.getMethod(), urlAcces, statusCode)) 
    {
        return (1);   
    }
    if (_request.getBody().size() > _serverConfig->getClientMaxBodySize()) 
    {
        res.setStatusCode(413);
        return (1);
    }
    if (urlAcces->getRedirectCode() != 0) 
    {
        res.setStatusCode(urlAcces->getRedirectCode());
        _redirectUrl = urlAcces->getRedirectUrl();
        return (1);
    }
    if (urlAcces->getPath().find("cgi-bin") != std::string::npos) 
    {
        _cgiFlag = 1;
        return (0);
    }
    _full_path = urlAcces->getRoot() + _request.getPath();
    if (!urlAcces->getCgiExtension().empty()) 
    {
        std::string ext = urlAcces->getCgiExtension();
        if (_full_path.length() >= ext.length() && 
            _full_path.compare(_full_path.length() - ext.length(), ext.length(), ext) == 0) {
            _cgiFlag = 1;
            return (0);
        }
    }
    if (isDirectory(_full_path)) 
    {
        if (_full_path.empty() || _full_path[_full_path.length() - 1] != '/') 
        {
            res.setStatusCode(301);
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
                res.setStatusCode(403);
                return (1);
            }
        }
    }
    if (!fileExist(_full_path)) 
    {
        res.setStatusCode(404);
        return (1);
    }
    return (0);
}

int Response::readFile(HTTPResponse &res)
{
    std::ifstream file(_full_path.c_str(), std::ios::binary | std::ios::ate);
    if (!file) 
    {
        res.setStatusCode(404);
        return (1);
    }   
    std::streamsize size = file.tellg();
    file.seekg(0, std::ios::beg);
    _body.resize(size);
    if (!file.read(reinterpret_cast<char*>(&_body[0]), size)) 
    {
        res.setStatusCode(500);
        return (1);
    }
    _contentLength = _body.size();
    return (0);
}

int Response::buildBody(HTTPResponse &res)
{
    if (buildPath(res))
    {
        buildErrorBody(res);
        return (1);
    }
    if (_cgiFlag || _autoindex)
        return (0);
    if (res.getStatusCode() != 0 && res.getStatusCode() != 200)
        return (0);
    std::string method_str = _request.getMethod();
    if (method_str == "GET")
    {
        if (readFile(res)) // Si falla la lectura, gestionem manualment
        {
            res.setStatusCode(404);
            buildErrorBody(res);
            return (1);
        }
    }
    else if (method_str == "POST" || method_str == "PUT") 
    {
        std::ofstream file(_full_path.c_str(), std::ios::binary);
        if (!file.is_open()) // Comprovació preventiva en lloc de catch
        {
            res.setStatusCode(500);
            buildErrorBody(res);
            return (1);
        }
        const std::vector<uint8_t>& rawBody = _request.getBody();
		std::string	contentType = _request.getHeader("Content-Type");
		bool isMultipart = (contentType.find("multipart/form-data") != std::string::npos);
		if (isMultipart)
		{
			std::string	boundary = extractBoundary(contentType);
			std::vector<uint8_t> cleanData = removeBoundary(_request.getBody(), boundary);
			file.write(reinterpret_cast<const char*>(cleanData.data()), cleanData.size());
		}
		else
		{
			const std::vector<uint8_t> &rawBody = _request.getBody();
			if (!rawBody.empty())
				file.write(reinterpret_cast<const char*>(rawBody.data()), rawBody.size());
		}
        file.close();
        res.setStatusCode(fileExist(_full_path) ? 200 : 201);
    }
    _contentLength = _body.size(); 
    if (res.getStatusCode() == 0)
        res.setStatusCode(200);
    return (0); // Tot ha anat bé
}

void    Response::buildErrorBody(HTTPResponse &res)
{
    if (_serverConfig && _serverConfig->getErrorPages().count(res.getStatusCode()))
    {
        _full_path = _location.getRoot() + _serverConfig->getErrorPages().at(res.getStatusCode());
        if (readFile(res))
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

void Response::buildResponse(HTTPResponse &res)
{
    if (buildBody(res))
        buildErrorBody(res);
    if (_cgiFlag)
        return;
    if (_autoindex)
    {
        if (buildHtmlIndex())
        {
            res.setStatusCode(500);
            buildErrorBody(res);
        }
        else
            res.setStatusCode(200);
    }
    setHeaders(res);
    if ((_request.getMethod() == "GET" || res.getStatusCode() != 200))
        _contentResponse.append(_body.begin(), _body.end());
}  

void    Response::cutResponse(size_t i)
{
    _contentResponse = _contentResponse.substr(i);
}


std::string Response::getHttpDate()
{
    std::string res;
    char buffer[100];
    time_t now = time (0);
    struct tm *tm = gmtime(&now);
    strftime(buffer, sizeof(buffer), "%a, %d %b %Y %H:%M:%S GMT", tm);
    res = buffer;
    return (res);
}

void Response::setHeaders(HTTPResponse &res)
{
    res.setVersion("HTTP/1.1");
    res.setHeader("Date", getHttpDate());
    res.setHeader("Server", "Webserv/42");
    std::stringstream ss;
    ss << _body.size();
    res.setHeader("Content-Length", ss.str());
    if (!_contentType.empty())
        res.setHeader("Content-Type", _contentType);
    else
        res.setHeader("Content-Type", "text/plain");
    if (_request.getHeader("connection") == "keep-alive")
        res.setHeader("connection", "keep-alive");
    short   code = res.getStatusCode();
    if (code >= 300 && code < 400 && !_redirectUrl.empty())
    {
        res.setHeader("Location", _redirectUrl);
    }
} 
