/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Utils.hpp                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: marvin <marvin@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/04/23 10:39:25 by marvin            #+#    #+#             */
/*   Updated: 2026/04/23 10:39:25 by marvin           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef UTILS_RESPONSE_HPP
#define UTILS_RESPONSE_HPP

#include "web_server.hpp"

std::string methodToStr(Methods method);
void findURL(const std::string &urlClient, const std::vector<LocationConfig> &urlLocs, std::string &urlWinner);
int  validMethods(Methods methods, const LocationConfig *local, short &code);
bool isDirectory(std::string &_urlDirect);
bool fileExist(std::string &_urlFile);
std::string codeToStr(short &code);
void fillMapCode(std::map<short, std::string> &_mapCode);
std::string statusCodeToString(short code);

#endif