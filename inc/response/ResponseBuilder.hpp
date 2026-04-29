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

#ifndef RESPONSEBUILDER_HPP
#define RESPONSEBUILDER_HPP

#include "HTTPResponse.hpp"
#include "../request/HTTPRequest.hpp"
#include "../parser/ServerConfig.hpp"
#include "../server/Server.hpp"
#include "../request/LocationConfig.hpp"
#include "../mime/MimeTypes.hpp"


class ResponseBuilder
{
    private:
            static int  resolvePath(const HTTPRequest& req, const ServerConfig& config, HTTPResponse& res);
            static int  readFile(const std::string& path, HTTPResponse& res);
            static int  buildAutoindex(const std::string& physicalPath, const std::string& virtualPath, HTTPResponse& res);
            static void executeCgi(const HTTPRequest& req, HTTPResponse& res);
            static void handleError(HTTPResponse& res, const ServerConfig& config);

    public:
            static void buildResponse(const HTTPRequest, const ServerConfig &conf, HTTPResponse &res);
};




#endif