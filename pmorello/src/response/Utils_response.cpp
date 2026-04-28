/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Utils_response.cpp                                 :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: pmorello <pmorello@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/04/23 10:38:27 by marvin            #+#    #+#             */
/*   Updated: 2026/04/28 10:44:16 by pmorello         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../pmorello/inc/response/Utils_response.hpp"

void findURL(const std::string &urlClient, const std::vector<LocationConfig> &urlLocs, std::string &urlWinner)
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

int  validMethods(const std::string  &methods, const LocationConfig *local, short &code)
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

bool isDirectory(std::string &_urlDirect)
{
    struct stat s;
    if (stat(_urlDirect.c_str(), &s) == 0)
        return (S_ISDIR(s.st_mode));
    return (false);
}

bool fileExist(std::string &_urlFile)
{
    struct stat s;
    return (stat(_urlFile.c_str(), &s) == 0);
}

std::string codeToStr(short &code)
{
    std::string str;
    std::stringstream ss;

    ss << code;
    ss >> str;
    return (str);
}

void fillMapCode(std::map<short, std::string> &_mapCode)
{
    _mapCode[100] = "Continue";
    _mapCode[101] = "Switching Protocol";
    _mapCode[200] = "OK";
    _mapCode[201] = "Created";
    _mapCode[202] = "Accepted";
    _mapCode[203] = "Non-Authoritative Information";
    _mapCode[204] = "No Content";
    _mapCode[205] = "Reset Content";
    _mapCode[206] = "Partial Content";
    _mapCode[300] = "Multiple Choice";
    _mapCode[301] = "Moved Permanently";
    _mapCode[302] = "Moved Temporarily";
    _mapCode[303] = "See Other";
    _mapCode[304] = "Not Modified";
    _mapCode[307] = "Temporary Redirect";
    _mapCode[308] = "Permanent Redirect";
    _mapCode[400] = "Bad Request";
    _mapCode[401] = "Unauthorized";
    _mapCode[403] = "Forbidden";
    _mapCode[404] = "Not Found";
    _mapCode[405] = "Method Not Allowed";
    _mapCode[406] = "Not Acceptable";
    _mapCode[407] = "Proxy Authentication Required";
    _mapCode[408] = "Request Timeout";
    _mapCode[409] = "Conflict";
    _mapCode[410] = "Gone";
    _mapCode[411] = "Length Required";
    _mapCode[412] = "Precondition Failed";
    _mapCode[413] = "Payload Too Large";
    _mapCode[414] = "URI Too Long";
    _mapCode[415] = "Unsupported Media Type";
    _mapCode[416] = "Requested Range Not Satisfiable";
    _mapCode[417] = "Expectation Failed";
    _mapCode[418] = "I'm a teapot";
    _mapCode[421] = "Misdirected Request";
    _mapCode[425] = "Too Early";
    _mapCode[426] = "Upgrade Required";
    _mapCode[428] = "Precondition Required";
    _mapCode[429] = "Too Many Requests";
    _mapCode[431] = "Request Header Fields Too Large";
    _mapCode[451] = "Unavailable for Legal Reasons";
    _mapCode[500] = "Internal Server Error";
    _mapCode[501] = "Not Implemented";
    _mapCode[502] = "Bad Gateway";
    _mapCode[503] = "Service Unavailable";
    _mapCode[504] = "Gateway Timeout";
    _mapCode[505] = "HTTP Version Not Supported";
    _mapCode[506] = "Variant Also Negotiates";
    _mapCode[507] = "Insufficient Storage";
    _mapCode[510] = "Not Extended";
    _mapCode[511] = "Network Authentication Required";
}

std::string statusCodeToString(short code)
{
    std::map<short, std::string> mapCode;
    if (mapCode.empty())
        fillMapCode(mapCode);
    std::map<short, std::string>::iterator it = mapCode.find(code);
    if (it != mapCode.end())
        return (it->second);
    return ("Undefined");
}