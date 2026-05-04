/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   CGIHandler.hpp                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: marvin <marvin@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/05/04 13:04:08 by marvin            #+#    #+#             */
/*   Updated: 2026/05/04 13:04:08 by marvin           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef CGHIHANDLER_HPP
#define CGIHANDLER_HPP

#include <iostream>  
#include <string>
#include <map>
#include <vector>
#include <sstream>
#include <algorithm>
#include <sys/types.h>
//#include <sys/wait.h>
#include <fcntl.h>
#include <unistd.h>     
#include <cstdlib>     
#include <cstring>
#include "../inc/CGI/CGIData.hpp"
#include "../inc/request/HTTPRequest.hpp"
#include "../inc/response/HTTPResponse.hpp"
#include "../inc/request/LocationConfig.hpp"


class CgiHandler 
{
    private:
            void _convertToCStyle(CGIData &data);
            std::string _decode(std::string path);

            CGIData     *_cgiData;
    
    public:
            CgiHandler();
            CgiHandler(const CgiHandler &other);
            ~CgiHandler();

    // Prepara el mapa d'entorn i l'argv
            void CgiHandler::prepare(CGIData &data, HTTPRequest &req, const std::string &scriptPath, const std::string &binPath);
    
    // Llança el procés (fork + execve)
            void execute(CGIData &data, short &error_code);

    // Mètodes asíncrons per al Server
            void writeToCgi(CGIData &data, const std::string &body);
            void readFromCgi(CGIData &data);


};


#endif