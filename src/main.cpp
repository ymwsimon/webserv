/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mayeung <mayeung@student.42london.com>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/05 19:29:27 by mayeung           #+#    #+#             */
/*   Updated: 2025/12/03 15:30:20 by mayeung          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../include/server.hpp"
#include "../include/utils.hpp"

int	main(int argc, char **argv)
{
	Server	server;
	std::string	head;
	std::string	body;
	std::string	res;

	head = getFullStatusMessage(500);
	body = head;
	appendHtmlTag("title", head);
	appendHtmlTag("head", head);
	appendHtmlTag("body", body);
	head.append(body);
	appendHtmlTag("html", head);
	std::cout << head << std::endl;
	// std::cout << body << std::endl;
	server.run();
	(void)argc;
	(void)argv;
	return 0;
}
