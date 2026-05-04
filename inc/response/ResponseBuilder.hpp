/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ResponseBuilder.hpp                                :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: marvin <marvin@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/04/29 13:12:45 by marvin            #+#    #+#             */
/*   Updated: 2026/04/29 13:12:45 by marvin           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef RESPONSE_BUILDER_HPP
#define RESPONSE_BUILDER_HPP

#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <map>
#include <sstream>
#include <sys/stat.h>
#include <dirent.h>
#include <algorithm>

#include "HTTPRequest.hpp"
#include "HTTPResponse.hpp"
#include "ServerConfig.hpp"
#include "LocationConfig.hpp"
#include "MimeTypes.hpp"

class Response 
{
    private:
            // Atributs de control i camins
            std::string             _full_path;
            std::string             _redirectUrl;
            int                     _cgiFlag;
            size_t                  _contentLength;
            bool                    _autoindex;
            std::string             _contentType;

        // Referències i dades de configuració
            MimeTypes               _mime;
            LocationConfig          _location;
            const HTTPRequest&       _request;
            const ServerConfig*      _serverConfig;
            HTTPResponse&            _response;

        // Mètodes privats interns (Lògica de decisió)
            int         buildPath();
            int         buildBody();
            void        buildErrorBody();
            int         buildHtmlIndex();
            void        setHeaders();
            int         readFile();

    public:
            Response(const HTTPRequest &req, HTTPResponse &res);
            Response(const Response &other);
            Response &operator=(const Response &other);
            ~Response();

        // Setters de configuració
            void        setMime(const MimeTypes& mime);
            void        setLocation(const LocationConfig& loc);
            void        setServerConfig(const ServerConfig* conf);

        // Mètode principal
            void        buildResponse();

        // Getters de control per al Server
            int         getCgiFlag() const;
            std::string getFullPath() const;
};

#endif