/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mayeung <mayeung@student.42london.com>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/05 19:29:27 by mayeung           #+#    #+#             */
/*   Updated: 2026/02/27 23:01:21 by mayeung          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../include/server.hpp"
#include "../include/utils.hpp"

bool	g_finish = false;
int		g_error = 0;

void	signalHandler(int sig)
{
	if (sig == SIGINT)
		g_finish = true;
}

int	main(int argc, char **argv)
{
	Server	server;
	std::string	configString;
	std::stringstream	ss;
	std::string			str;
	
	signal(SIGINT, signalHandler);
	if (readConfigFile("test.conf", configString))
	{
		std::cout << "read ok" << std::endl;
		// std::cout << ss;
		// while (!ss.eof())
		// {
		// 	std::string	str;

		// 	std::getline(ss, str);
		// 	std::cout << str << std::endl;
		// }
	}
	else
		std::cout << "read file error" << std::endl;
	ss << configString;
	if (serverConfigParser(ss, server, str))
	{
		std::cout << "server parse ok"<<std::endl;
		server.printConfig();
	}
	else
		std::cout << "server parse not ok"<<std::endl;
	// {
	
	// 	Config				c;
	// 	// Location			l;
		
	// 	ss << configString;
	// 	// ss << "location / {\n autoIndex on; }\n";
	// 	if (!configParser(ss, c, str))
	// 		std::cout << "no" << std::endl;
	// 	else
	// 	{
	// 		std::cout << "ok" << std::endl;
	// 		c.printConfig();
	// 		// l.printLocation();
	// 	}
	// 	// std::getline(ss, str, ' ');
	// 	// std::cout << str;// << std::endl;
	// }
	
	// server.run();
	(void)argc;
	(void)argv;
	return g_error;
}
