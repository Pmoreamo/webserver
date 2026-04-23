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
#include "../rmanzana/inc/ConfigParser.hpp"
#include "../rmanzana/inc/LocationConfig.hpp"
#include "../rmanzana/inc/Server.hpp"
#include "../rmanzana/inc/ServerConfig.hpp"
#include "../rmanzana/inc/Utils.hpp"
#include "../rmanzana/inc/Mime(mock).hpp"

#include "Response.hpp"
#include "Utils_response.hpp"

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

#endif