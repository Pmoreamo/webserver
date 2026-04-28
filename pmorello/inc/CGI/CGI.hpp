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

#ifndef CGI_HPP
#define CGI_HPP

#include <iostream>  
#include <string>
#include <map>
#include <vector>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>     
#include <cstdlib>     
#include <cstring>      
#include "../davifer2/includes/HTTPRequest.hpp"

class CGI
{
    private:
        std::string                         _CGIpath;
        std::string                         _responseContent;
        std::map<std::string, std::string>  _env;
        char                                **_chEnv;
        char                                **_args;
        int                                 _exitStatus;
        pid_t                               _CGIpid;

    public:
        CGI();
        CGI(std::string path);
        CGI(const CGI &src);
        CGI &operator=(const CGI &src);
        ~CGI();

        void    setCGIPath(const std::string &path);
        void    initEnv(HTTPRequest &req, const std::string &scriptPath, const std::string &binPath);
        void    execute(short &error_code, const std::string &body);
        void    clear();

        const std::string   &getOutput() const;
        int                 getExitStatus() const;
        pid_t               getCGIPid() const;
};

#endif