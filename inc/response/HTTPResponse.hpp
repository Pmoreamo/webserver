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

#ifndef HTTPRESPONSE_HPP
#define HTTPRESPONSE_HPP

#include <iostream>
#include <string>
#include <vector>
#include <map>
#include <stdint.h>
#include <sstream>
#include "../utils/Utils.hpp"
    
class HTTPResponse {
        private:
                short                                   _statusCode;
                std::string                             _httpVersion;
                std::map<std::string, std::string>      _headers;
                std::vector<uint8_t>                      _body;
                std::string                             _fullResponse;

        public:
                HTTPResponse();
                ~HTTPResponse();
                HTTPResponse(const HTTPResponse& other);
                HTTPResponse& operator=(const HTTPResponse& other);

                // Getters
                short                                           getStatusCode() const;
                const std::string&                              getVersion() const;
                const std::map<std::string, std::string>        &getHeaders() const;
                const std::vector<uint8_t>&                     getBody() const;
                

                // Setters
                void    setStatusCode(short code);
                void    setVersion(const std::string& version);
                void    setHeader(std::string name, const std::string& value);
                void    setBody(const std::vector<uint8_t>& body);
 

                // Utils
                void    appendBody(const std::vector<uint8_t> &data);
                void    appendBody(const std::string &data);
                void    clear();
                void    HTTPResponse::packResponse();
                
};

#endif