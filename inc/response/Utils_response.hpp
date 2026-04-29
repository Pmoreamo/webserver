/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Utils_response.hpp                                 :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: pmorello <pmorello@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/04/23 10:39:25 by marvin            #+#    #+#             */
/*   Updated: 2026/04/28 10:41:21 by pmorello         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef UTILS_RESPONSE_HPP
#define UTILS_RESPONSE_HPP

#include <iostream>
#include <string>
#include <sys/stat.h>
#include "../davifer2/includes/HTTPRequest.hpp"
#include "../rmanzana/inc/parser/ConfigParser.hpp"
#include "../rmanzana/inc/parser/LocationConfig.hpp"
#include "../rmanzana/inc/server/Server.hpp"
#include "../rmanzana/inc/parser/ServerConfig.hpp"
#include "../rmanzana/inc/utils/Utils.hpp"
#include "../davifer2/includes/MimeTypes.hpp"
#include "../pmorello/inc/CGI/CGI.hpp"

void findURL(const std::string &urlClient, const std::vector<LocationConfig> &urlLocs, std::string &urlWinner);
int  validMethods(const std::string  &methods, const LocationConfig *local, short &code);
bool isDirectory(std::string &_urlDirect);
bool fileExist(std::string &_urlFile);
std::string codeToStr(short &code);
void fillMapCode(std::map<short, std::string> &_mapCode);
std::string statusCodeToString(short code);

#endif