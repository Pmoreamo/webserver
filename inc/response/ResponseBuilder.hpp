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

#ifndef RESPONSE_HPP
#define RESPONSE_HPP

#include "../request/HTTPRequest.hpp"
#include "../request/LocationConfig.hpp"
#include "../server/Server.hpp"
#include "../parser/ServerConfig.hpp"
#include "../response/HTTPResponse.hpp"
#include "../mime/MimeTypes.hpp"
#include "../CGI/CGI.hpp"
#include "../utils/Utils.hpp"

class Response
{
   
        private:
                std::string             _full_path; //Ruta absoluta/relativa al sistema.
                std::vector<uint8_t>    _body; //contingut en Vector de bytes (binari pur).
                std::string             _redirectUrl; //URL de destí (Header Location).
                int                     _cgiFlag; //flag d'activació.
                size_t                  _contentLength; //longitud del contingut en bytes.
                bool                    _autoindex; //Interruptor de per mostrar llistat o nomes 1
                std::string             _contentType;
                std::string             _contentResponse;

                MimeTypes               _mime;
                LocationConfig          _location; //copia de Location
                const HTTPRequest       &_request; //copia de Request
                const Server            *_server; //punter al server
                const ServerConfig      *_serverConfig; //punter a la configuracio del server
                const HTTPResponse       &_response;

                

        public:
                Response(const HTTPRequest &req, const HTTPResponse &res);
                Response(const Response &src);
                Response        &operator=(const Response &src);
                ~Response();

                //setters
                void setMime(const MimeTypes& mime);
                void setLocation(const LocationConfig& loc);
                void setServer(const Server* serv);
                void setServerConfig(const ServerConfig* conf);

                //logica
                int                     buildPath(HTTPResponse &res);
                int                     buildBody(HTTPResponse &res);
                int                     readFile(HTTPResponse &res);
                void                    buildErrorBody(HTTPResponse &res);
                void                    setHeaders(HTTPResponse &res);
                void                    buildResponse(HTTPResponse &res);
                int                     buildHtmlIndex();
                void                    cutResponse(size_t i);
                std::string             getHttpDate();
           
};

#endif