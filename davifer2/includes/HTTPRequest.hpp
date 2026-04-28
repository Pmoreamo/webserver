/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   HTTPRequest.hpp                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: pmorello <pmorello@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/04/28 11:26:16 by pmorello          #+#    #+#             */
/*   Updated: 2026/04/28 11:27:48 by pmorello         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef HTTPREQUEST_HPP
#define HTTPREQUEST_HPP

# include <string>
# include <map>

class HTTPRequest
{
public:
	HTTPRequest();
	~HTTPRequest();
	HTTPRequest(const HTTPRequest& other);
	HTTPRequest& operator=(const HTTPRequest& other);

	// Getters
	const std::string&							getMethod() const;
	const std::string&							getUri() const;
	const std::string&							getPath() const;
	const std::string&							getQueryString() const;
	const std::string&							getHttpVersion() const;
	const std::map<std::string, std::string>&	getHeaders() const;
	std::string									getHeader(const std::string& name) const;
	const std::string&							getBody() const;
	int											getErrorCode() const;
	bool										getIsComplete() const;

	// Setters
	void	setMethod(const std::string& method);
	void	setUri(const std::string& uri);
	void	setPath(const std::string& path);
	void	setQueryString(const std::string& queryString);
	void	setHttpVersion(const std::string& version);
	void	setHeader(const std::string& name, const std::string& value);
	void	setBody(const std::string& body);
	void	appendBody(const std::string& data);
	void	setErrorCode(int code);
	void	setIsComplete(bool complete);

	// Utils
	bool	hasHeader(const std::string& name) const;
	bool	shouldKeepAlive() const;
	void	clear();

private:
	std::string							_method;
	std::string							_uri;
	std::string							_path;
	std::string							_queryString;
	std::string							_httpVersion;
	std::map<std::string, std::string>	_headers;
	std::string							_body;
	int									_errorCode;
	bool								_isComplete;
};

#endif
