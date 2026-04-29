#include "HTTPRequest.hpp"
#include <cctype>

HTTPRequest::HTTPRequest()
	: _errorCode(0), _isComplete(false)
{
}

HTTPRequest::~HTTPRequest()
{
}

HTTPRequest::HTTPRequest(const HTTPRequest& other)
	: _method(other._method),
	  _uri(other._uri),
	  _path(other._path),
	  _queryString(other._queryString),
	  _httpVersion(other._httpVersion),
	  _headers(other._headers),
	  _body(other._body),
	  _errorCode(other._errorCode),
	  _isComplete(other._isComplete)
{
}

HTTPRequest& HTTPRequest::operator=(const HTTPRequest& other)
{
	if (this != &other)
	{
		_method = other._method;
		_uri = other._uri;
		_path = other._path;
		_queryString = other._queryString;
		_httpVersion = other._httpVersion;
		_headers = other._headers;
		_body = other._body;
		_errorCode = other._errorCode;
		_isComplete = other._isComplete;
	}
	return *this;
}

// Getters
const std::string& HTTPRequest::getMethod() const { return _method; }
const std::string& HTTPRequest::getUri() const { return _uri; }
const std::string& HTTPRequest::getPath() const { return _path; }
const std::string& HTTPRequest::getQueryString() const { return _queryString; }
const std::string& HTTPRequest::getHttpVersion() const { return _httpVersion; }
const std::map<std::string, std::string>& HTTPRequest::getHeaders() const { return _headers; }
const std::string& HTTPRequest::getBody() const { return _body; }
int HTTPRequest::getErrorCode() const { return _errorCode; }
bool HTTPRequest::getIsComplete() const { return _isComplete; }

std::string HTTPRequest::getHeader(const std::string& name) const
{
	std::map<std::string, std::string>::const_iterator it = _headers.find(name);
	if (it != _headers.end())
		return it->second;
	return "";
}

// Setters
void HTTPRequest::setMethod(const std::string& method) { _method = method; }
void HTTPRequest::setUri(const std::string& uri) { _uri = uri; }
void HTTPRequest::setPath(const std::string& path) { _path = path; }
void HTTPRequest::setQueryString(const std::string& queryString) { _queryString = queryString; }
void HTTPRequest::setHttpVersion(const std::string& version) { _httpVersion = version; }
void HTTPRequest::setBody(const std::string& body) { _body = body; }
void HTTPRequest::setErrorCode(int code) { _errorCode = code; }
void HTTPRequest::setIsComplete(bool complete) { _isComplete = complete; }

void HTTPRequest::setHeader(const std::string& name, const std::string& value)
{
	_headers[name] = value;
}

void HTTPRequest::appendBody(const std::string& data)
{
	_body.append(data);
}

// Utils
bool HTTPRequest::hasHeader(const std::string& name) const
{
	return _headers.find(name) != _headers.end();
}

bool HTTPRequest::shouldKeepAlive() const
{
	std::string connection = getHeader("connection");
	// Connection header value is case-insensitive (RFC 7230)
	for (size_t i = 0; i < connection.size(); ++i)
		connection[i] = static_cast<char>(
			std::tolower(static_cast<unsigned char>(connection[i])));
	if (_httpVersion == "HTTP/1.1")
		return connection != "close";
	return connection == "keep-alive";
}

void HTTPRequest::clear()
{
	_method.clear();
	_uri.clear();
	_path.clear();
	_queryString.clear();
	_httpVersion.clear();
	_headers.clear();
	_body.clear();
	_errorCode = 0;
	_isComplete = false;
}
