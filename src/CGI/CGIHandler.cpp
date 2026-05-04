/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   CGIHandler.cpp                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: marvin <marvin@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/05/04 13:06:39 by marvin            #+#    #+#             */
/*   Updated: 2026/05/04 13:06:39 by marvin           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../inc/CGI/CGIHandler.hpp"

CgiHandler::CgiHandler() {}
CgiHandler::~CgiHandler() {}

void CgiHandler::prepare(CGIData &data, HTTPRequest& req, const std::string& scriptPath, const std::string& binPath) 
{
    data._cgiPath = scriptPath;

    // 1. Variables d'entorn bàsiques
    data._env["GATEWAY_INTERFACE"] = "CGI/1.1";
    data._env["SERVER_PROTOCOL"]   = "HTTP/1.1";
    data._env["REDIRECT_STATUS"]   = "200";
    data._env["REQUEST_METHOD"]    = req.getMethod();
    data._env["SCRIPT_FILENAME"]   = scriptPath;
    data._env["QUERY_STRING"]      = req.getQueryString();
    data._env["CONTENT_TYPE"]      = req.getHeader("content-type");
    
    std::stringstream ss;
    ss << req.getBody().length();
    data._env["CONTENT_LENGTH"]    = ss.str();

    // 2. Convertir mapa a char** per l'execve
    _convertToCStyle(data);

    // 3. Preparar argv
    data._args = (char**)malloc(sizeof(char*) * 3);
    data._args[0] = strdup(binPath.c_str());
    data._args[1] = strdup(scriptPath.c_str());
    data._args[2] = NULL;
}

void CgiHandler::execute(CGIData& data, HTTPResponse &res) 
{
    if (pipe(data._pipeIn) < 0 || pipe(data._pipeOut) < 0) 
    {
        res.setStatusCode(500);
        return;
    }
    data._cgiPid = fork();
    if (data._cgiPid == 0) { // Fill
        dup2(data._pipeIn[0], STDIN_FILENO);
        dup2(data._pipeOut[1], STDOUT_FILENO);
        
        // Tanquem tot el que no fem servir al fill
        close(data._pipeIn[1]);
        close(data._pipeOut[0]);

        execve(data._args[0], data._args, data._chEnv);
        exit(1); // Si arriba aquí, error
    } 
    else if (data._cgiPid < 0) 
        res.setStatusCode(500);
    // Pare: Tanquem els extrems que farà servir el fill
    close(data._pipeIn[0]);
    close(data._pipeOut[1]);
    data._pipeIn[0] = -1;
    data._pipeOut[1] = -1;
}

void CgiHandler::writeToCgi(CGIData& data, const std::string& body) 
{
    if (body.empty()) 
    {
        close(data._pipeIn[1]);
        data._pipeIn[1] = -1;
        return;
    }
    write(data._pipeIn[1], body.c_str(), body.length());
    close(data._pipeIn[1]); // Tanquem després d'escriure el POST
    data._pipeIn[1] = -1;
}

void CgiHandler::readFromCgi(CGIData& data) 
{
    char buffer[4096];
    int bytes = read(data._pipeOut[0], buffer, sizeof(buffer));
    
    if (bytes > 0) 
    {
        data._response.append(buffer, bytes);
    } 
    else 
    {
        data._isFinished = true;
        close(data._pipeOut[0]);
        data._pipeOut[0] = -1;
    }
}

void CgiHandler::_convertToCStyle(CGIData& data) 
{
    data._chEnv = new char*[data._env.size() + 1];
    int i = 0;
    for (std::map<std::string, std::string>::iterator it = data._env.begin(); it != data._env.end(); it++) 
    {
        std::string tmp = it->first + "=" + it->second;
        data._chEnv[i] = ft_strcpy(data._chEnv[i], tmp.c_str());
        i++;
    }
    data._chEnv[i] = NULL;
}