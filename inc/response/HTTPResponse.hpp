/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Response.hpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: pmorello <pmorello@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/04/14 13:18:16 by pmorello          #+#    #+#             */
/*   Updated: 2026/04/28 10:44:08 by pmorello         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef RESPONSE_HPP
#define RESPONSE_HPP

#include <iostream>
#include <string>
#include <vector>
#include <fstream> 
#include <stdint.h>     
#include <sys/stat.h>   
#include <dirent.h>     

#include 

class Response
{
   
        private:
                std::string             _full_path; //Ruta absoluta/relativa al sistema.
                std::vector<uint8_t>    _body; //contingut en Vector de bytes (binari pur).
                short                   _redirectCode; //Codi d'estat HTTP (Status Code).
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
                CGI                     _cgi;


                int buildCGI();
        public:
                Response(const HTTPRequest &req);
                Response(const Response &src);
                Response        &operator=(const Response &src);
                ~Response();

                void    setServer(const Server *serv);
                void    setServerConfig(const ServerConfig *servConf);


                const   std::string     &getFullPath() const;
                const   std::vector<uint8_t> &getBody() const;
                const   std::string     &getRedirectUrl() const;
                int     getCGIflag();
                bool    getAutoindex();
                
                std::string     getResponse() const;
                short           getRedirectCode();
                size_t          getContentLength();

                int buildPath();
                int buildBody();
                int readFile();
                std::vector<uint8_t> removeBoundary(const std::vector<uint8_t>&_body, const std::string &boundary);
                void    buildErrorBody();
                std::string getFullResponse();
                void    setHeaders();
                void    buildResponse();
                int     buildHtmlIndex();
                void    cutResponse(size_t i);
                void    clear();



                class BodyEmptyException : public std::exception
                {
                        public:
                                virtual const char *what() const throw();
                };
            
};

#endif



