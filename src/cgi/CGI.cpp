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

#include "../pmorello/inc/CGI/CGI.hpp"

CGI::CGI() : _chEnv(NULL), _args(NULL), _exitStatus(0), _CGIpid(-1) {}

CGI::CGI(std::string path) : _CGIpath(path), _chEnv(NULL), _args(NULL), _exitStatus(0), _CGIpid(-1) {}

CGI::CGI(const CGI &src) : _chEnv(NULL), _args(NULL) { *this = src; }

CGI &CGI::operator=(const CGI &src) {
    if (this != &src) {
        clear();
        _CGIpath = src._CGIpath;
        _exitStatus = src._exitStatus;
        _CGIpid = src._CGIpid;
    }
    return (*this);
}

CGI::~CGI() { clear(); }

void CGI::clear() {
    if (_chEnv) {
        for (int i = 0; _chEnv[i]; i++) delete[] _chEnv[i];
        delete[] _chEnv;
        _chEnv = NULL;
    }
    if (_args) {
        for (int i = 0; _args[i]; i++) delete[] _args[i];
        delete[] _args;
        _args = NULL;
    }
    _env.clear();
    _responseContent.clear();
}

void CGI::initEnv(Request &req, const std::string &scriptPath, const std::string &binPath) {
    clear();
    _CGIpath = scriptPath;

    _env["GATEWAY_INTERFACE"] = "CGI/1.1";
    _env["SERVER_PROTOCOL"] = "HTTP/1.1";
    _env["REQUEST_METHOD"] = req.getMethodStr();
    _env["SCRIPT_FILENAME"] = scriptPath;
    _env["QUERY_STRING"] = req.getQuery();
    _env["REDIRECT_STATUS"] = "200";
    _env["CONTENT_TYPE"] = req.getHeader("content-type");
    _env["CONTENT_LENGTH"] = req.getHeader("content-length");

    // Convertir map a char**
    _chEnv = new char*[_env.size() + 1];
    int i = 0;
    for (std::map<std::string, std::string>::iterator it = _env.begin(); it != _env.end(); ++it) {
        std::string entry = it->first + "=" + it->second;
        _chEnv[i] = new char[entry.length() + 1];
        std::strcpy(_chEnv[i], entry.c_str());
        i++;
    }
    _chEnv[i] = NULL;

    // Preparar arguments per execve
    _args = new char*[3];
    _args[0] = new char[binPath.length() + 1];
    std::strcpy(_args[0], binPath.c_str());
    _args[1] = new char[scriptPath.length() + 1];
    std::strcpy(_args[1], scriptPath.c_str());
    _args[2] = NULL;
}

void CGI::execute(short &error_code, const std::string &body) {
    int pipeIn[2];
    int pipeOut[2];

    if (pipe(pipeIn) < 0 || pipe(pipeOut) < 0) {
        error_code = 500;
        return;
    }

    _CGIpid = fork();
    if (_CGIpid < 0) {
        error_code = 500;
        return;
    }

    if (_CGIpid == 0) {
        dup2(pipeIn[0], STDIN_FILENO);
        dup2(pipeOut[1], STDOUT_FILENO);
        close(pipeIn[1]); close(pipeOut[0]);
        close(pipeIn[0]); close(pipeOut[1]);
        execve(_args[0], _args, _chEnv);
        exit(1);
    } else {
        close(pipeIn[0]); close(pipeOut[1]);
        if (!body.empty())
            write(pipeIn[1], body.c_str(), body.length());
        close(pipeIn[1]);

        char buffer[4096];
        int bytes;
        while ((bytes = read(pipeOut[0], buffer, sizeof(buffer))) > 0)
            _responseContent.append(buffer, bytes);
        close(pipeOut[0]);
        waitpid(_CGIpid, &_exitStatus, 0);
        if (WIFEXITED(_exitStatus) && WEXITSTATUS(_exitStatus) != 0)
            error_code = 502;
    }
}

const std::string &CGI::getOutput() const { return _responseContent; }
