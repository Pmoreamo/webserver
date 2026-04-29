/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: pmorello <pmorello@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/04/14 13:16:48 by pmorello          #+#    #+#             */
/*   Updated: 2026/04/14 13:18:02 by pmorello         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "web_server.hpp"

int main() {
    Server config;
    Request req;
    Response res;

    res.setServer(config);
    res.setRequest(req);
    res.buildResponse();

    std::cout << res.getRes() << std::endl; // Hauria d'imprimir el 200 OK i l'HTML
    return 0;
}