/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   CGI.cpp                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: marvin <marvin@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/04/27 09:56:39 by marvin            #+#    #+#             */
/*   Updated: 2026/04/27 09:56:39 by marvin           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "web_server.hpp"

CGI::CGI(const Request &src) : _envp(NULL), _req(src) 
{
    _path_script = "";
    _cgiOutput = "";
}

CGI::CGI(const CGI &src) : _envp(src._envp), _req(src._req)
{
    *this = src;
}

CGI &CGI::operator=(const CGI &src)
{
    if (this != &src)
    {
        _path_script = src._path_script;
        _env_map = src._env_map;
        _envp = src._envp;
        _cgiOutput = src._cgiOutput;
    }
    return (*this);
}

CGI::~CGI() {}

void    CGI::setServerConfig(const ServerConfig *servConf)
{
    this->_servConf = servConf;
}

void    CGI::initEnv(const Request &req, const ServerConfig *config)
{
    _env_map["REQUEST_METHOD"] = methodToStr(req.getMethods());
    _env_map["SCRIPT_FILENAME"] = _path_script;
    _env_map["SERVER_PROTOCOL"] = "HTTP/1.1";
    _env_map["REDIRECT_STATUS"] = "200";

    if (req.getMethods() == POST)
    {
        std::stringstream ss;
        ss << req.getBodyString().length();
        _env_map["CONTENT_LENGTH"] = ss.str();
        _env_map["CONTENT_TYPE"] = "application/x-www-form-urlencoded";
    }
    _mapToEnvp();
}

void    CGI::_mapToEnvp()
{
    _envp = new char*[_env_map.size() + 1];
    int i = 0;
    for (std::map<std::string, std::string>::iterator it = _env_map.begin(); it != _env_map.end(); it++)
    {
        std::string entry = it->first + "=" + it->second;
        _envp[i] = new char[entry.length() + 1];
        entry.copy(_envp[i], entry.length());
        _envp[i][entry.length()] = '\0';
        i++;
    }
    _envp[i] = NULL;
}


int CGI::execute()
{
    int _pipeOut[2];
    if (pipe(_pipeOut) < 0)
        return 500;
    pid_t pid = fork();
    if (pid < 0)
        return (500);
    if (pid == 0)
    {
        dup2(_pipeOut[1], STDOUT_FILENO);
        close(_pipeOut[0]);
        close(_pipeOut[1]);

        char    *args[] = {(char*)_path_script.c_str(), NULL};
        execve(args[0], args, _envp);
        exit(1);
    }
    else
    {
        close(_pipeOut[1]);
        char buffer[4096];
        int bytes;
        while ((bytes = read(_pipeOut[0], buffer, 4096)) > 0)
        {
            _cgiOutput.append(buffer, bytes);
        }
        close(_pipeOut[0]);
        waitpid(pid, NULL, 0);
    }
    return (200);
}

std::string CGI::getOutput() const
{
    return (this->_cgiOutput);
}