/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   web_server.hpp                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: pmorello <pmorello@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/04/14 17:12:35 by pmorello          #+#    #+#             */
/*   Updated: 2026/04/16 13:08:12 by pmorello         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef WEB_SERVER_HPP
#define WEB_SERVER_HPP

#include "../rmanzana/inc/Request(mock).hpp"
#include "../rmanzana/inc/parser/ConfigParser.hpp"
#include "../rmanzana/inc/parser/LocationConfig.hpp"
#include "../rmanzana/inc/server/Server.hpp"
#include "../rmanzana/inc/parser/ServerConfig.hpp"
#include "../rmanzana/inc/utils/Utils.hpp"
#include "../rmanzana/inc/Mime(mock).hpp"

#include "Response.hpp"
#include "Utils_response.hpp"
#include "CGI.hpp"

#include <iostream>
#include <string>
#include <map>
#include <vector>
#include <sys/stat.h>
#include <fstream>
#include <sstream>
#include <algorithm>
#include <dirent.h>
#include <sys/types.h>
#include <cstdint>
#include <sys/wait.h>
#include <unistd.h>

#endif