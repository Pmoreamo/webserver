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
#include <sstream>   

#include "../request/HTTPRequest.hpp"
#include "../request/LocationConfig.hpp"
#include "../server/Server.hpp"
#include "../parser/ServerConfig.hpp"
#include "../mime/MimeTypes.hpp"
#include "../CGI/CGI.hpp"
#include "../utils/Utils.hpp"

//nova calsse HTTPResponse, les part de dades
class HTTPResponse
{
        private:
                short                                   _statusCode;
                std::string                             _statusMsg;
                std::string                             _httpVersion;
                std::map<std::string, std::string>      _headers;
                std::vector<char*>                      _body;
                std::string                             _fullResponse;
        public:
                HTTPResponse();
                HTTPResponse(const HTTPResponse &other);
                HTTPResponse    &operator=(const HTTPResponse &other);
                ~HTTPResponse();

                //getters

                //setters
                
};

#endif



