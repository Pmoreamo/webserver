/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   CGI.hpp                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: marvin <marvin@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/04/27 09:52:29 by marvin            #+#    #+#             */
/*   Updated: 2026/04/27 09:52:29 by marvin           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef CGI_HPP
#define CGI_HPP

#include "web_server.hpp"

class CGI
{
    private:
            std::string _path_script;
            std::map<std::string, std::string> _env_map;
            char    **_envp;
            std::string _cgiOutput;

            const   Request &_req;
            const   ServerConfig *_servConf;

            void    initEnv(const Request &req, const ServerConfig *config);
            void    _mapToEnvp();
    public:
            CGI(const CGI &src);
            CGI(const Request &src);
            CGI &operator=(const CGI &src);
            ~CGI();
            
            void    setServerConfig(const ServerConfig *servConf);
            
            int execute();
            std::string getOutput() const;
            void    clear();
};


#endif