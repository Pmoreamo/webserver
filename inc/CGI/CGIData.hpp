/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   CGI.hpp                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: pmorello <pmorello@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/04/27 09:52:29 by marvin            #+#    #+#             */
/*   Updated: 2026/04/28 10:25:10 by pmorello         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef CGIDATA_HPP
#define CGDATA_HPP

 
#include <string>
#include <map>
#include <vector>
#include "../inc/request/HTTPRequest.hpp"
#include "../inc/response/HTTPResponse.hpp"
#include "../inc/request/LocationConfig.hpp"

class CGIData 
{
	    public:
		        std::map<std::string, std::string>	_env;
		        char**								_chEnv;
		        char**								_args;
		        int									_exitStatus;
		        std::string							_cgiPath;
				std::string							_response;
		        pid_t								_cgiPid;
				bool								_isFinished;

        
		        int	_pipeIn[2];
		        int	_pipeOut[2];

		        CGIData();
		        CGIData(const CGIData &other);
		        CGIData	&operator=(const CGIData &other);
                ~CGIData();

				//setters
				void	setEnv(const std::map<std::string, std::string> &env);
				void	setCgiPath(const std::string &path);

				//getters
				std::map<std::string, std::string> &getEnv();
				const std::string	&getCgiPath() const;

};

#endif