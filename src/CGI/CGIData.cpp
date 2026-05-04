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

#include "../inc/CGI/CGIData.hpp"

CGIData::CGIData() : 
    _env("", ""), 
    _chEnv(NULL), 
    _args(NULL),
    _exitStatus(0), 
    _cgiPath(""), 
    _cgiPid(-1)
{}

CGIData::CGIData(const CGIData &other) : 
    _env(other._env),
    _chEnv(other._chEnv),
    _args(other._args),
    _exitStatus(other._exitStatus),
    _cgiPath(other._cgiPath),
    _cgiPid(other._cgiPid)
{}

CGIData &CGIData::operator=(const CGIData &other)
{
    if (this != &other)
    {
        _env = other._env;
        _chEnv = other._chEnv;
        _args = other._args;
        _exitStatus = other._exitStatus;
        _cgiPath = other._cgiPath;
        _cgiPid = other._cgiPid;
    }
    return (*this);
}

//setters
void    CGIData::setEnv(const std::map<std::string, std::string> &env)
{
    this->_env = env;
}

void    CGIData::setCgiPath(const std::string &path)
{
    this->_cgiPath = path;
}

//getters
std::map<std::string, std::string>  &CGIData::getEnv()
{
    return (this->_env);
}

const std::string   &CGIData::getCgiPath() const
{
    return (this->_cgiPath);
}


