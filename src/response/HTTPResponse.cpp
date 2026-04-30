/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Response.cpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: pmorello <pmorello@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/04/16 10:37:25 by pmorello          #+#    #+#             */
/*   Updated: 2026/04/28 10:44:28 by pmorello         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "HTTPResponse.hpp"

HTTPResponse::HTTPResponse() :
    _statusCode(0), 
    _httpVersion(""), 
    _headers("",""),
    _body(0),
    _fullResponse("")
{

}

HTTPResponse::HTTPResponse(const HTTPResponse &other) :
    _statusCode(other._statusCode), 
    _httpVersion(other._httpVersion), 
    _headers(other._headers),
    _body(other._body),
    _fullResponse(other._fullResponse)
{

}

HTTPResponse    &HTTPResponse::operator=(const HTTPResponse &other)
{
    if (this != &other)
    {
        _statusCode = other._statusCode; 
        _httpVersion = other._httpVersion; 
        _headers = other._headers;
        _body = other._body;
        _fullResponse = other._fullResponse;
    }
    return (*this);
}

HTTPResponse::~HTTPResponse()
{

}

//getters
short   HTTPResponse::getStatusCode() const
{
    return (this->_statusCode);
}

const std::string   &HTTPResponse::getVersion() const
{
    return (this->_httpVersion);
}

const std::map<std::string, std::string>    &HTTPResponse::getHeaders() const
{
    return (this->_headers);
}

const std::vector<uint8_t>    &HTTPResponse::getBody() const
{
    return (this->_body);
}


//setters
void    HTTPResponse::setStatusCode(short code)
{
    this->_statusCode = code;
}

void    HTTPResponse::setVersion(const std::string& version)
{
    this->_httpVersion = version;
}

void    HTTPResponse::setHeader(std::string name, const std::string& value)
{
    this->_headers[name] = value;
}

void    HTTPResponse::setBody(const std::vector<uint8_t>& body)
{
    this->_body = body;
}

//utils
void    HTTPResponse::appendBody(const std::vector<uint8_t> &data)
{
    _body.insert(_body.end(), data.begin(), data.end());
}

void    HTTPResponse::appendBody(const std::string &data)
{
    _body.insert(_body.end(), data.begin(), data.end());
}

void    HTTPResponse::clear()
{
    _statusCode = 0;
    _httpVersion.clear();
    _headers.clear();
    _body.clear();
    _fullResponse.clear();
}

void HTTPResponse::packResponse() 
{
    std::stringstream ss;
    ss << _httpVersion << " " << _statusCode << " " << statusCodeToString(_statusCode) << "\r\n";
    for (std::map<std::string, std::string>::iterator it = _headers.begin(); it != _headers.end(); ++it) 
    {
        ss << it->first << ": " << it->second << "\r\n";
    }
    ss << "\r\n";
    _fullResponse = ss.str();
    if (!_body.empty()) 
    {
        _fullResponse.append(reinterpret_cast<const char*>(_body.data()), _body.size());
    }
}