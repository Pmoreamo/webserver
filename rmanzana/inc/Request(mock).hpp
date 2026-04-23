/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Request.hpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: pmorello <pmorello@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/04/14 13:15:54 by pmorello          #+#    #+#             */
/*   Updated: 2026/04/16 13:05:40 by pmorello         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef REQUEST_HPP
#define REQUEST_HPP

#include "web_server.hpp"

enum Methods 
{
    GET,
    POST,
    DELETE,
    PUT
};

class Request {

    public:
        std::string path;
        Methods _methods;
        std::string _boundary;
        std::vector<uint8_t> _body;

    const Methods &getMethods() const {return this->_methods; }
    const std::string &getPath() const { return this->path; } // Prova a canviar-ho per testejar rutes
    const std::string &getBodyString() const { return ""; }
    const std::vector<uint8_t> &getBodyBits() const { return this->_body; }
    const std::string &getHeader(std::string key) const { return "keep-alive"; }
    int         errorCode() { return 0; }
    const std::string &getBoundary() const { return this->_boundary; }
    const bool        &getMultiformFlag() const { return false; }
};

#endif