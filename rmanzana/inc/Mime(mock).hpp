/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Mime(mock).hpp                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: marvin <marvin@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/04/17 11:07:50 by marvin            #+#    #+#             */
/*   Updated: 2026/04/17 11:07:50 by marvin           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef MIME_HPP
#define MIME_HPP

#include "../pmorello/web_server.hpp"

class Mime
{
    public:
        Mime();
        std::string getMimeType(std::string extension);
    private:
        std::map<std::string, std::string> _mime_types;
        
};

#endif// MIME_HPP