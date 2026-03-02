/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mayeung <mayeung@student.42london.com>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/05 19:29:27 by mayeung           #+#    #+#             */
/*   Updated: 2026/03/02 10:16:27 by mayeung          ###   ########.fr       */
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
	Server		server;
	std::string	configString;
	std::string	path;
	
	signal(SIGINT, signalHandler);
	if (argc == 1 || argc == 2)
	{
		if (argc == 1)
			path = "config/test.conf";
		else if (argc == 2)
			path = argv[1];
		if (readConfigFile(path, configString))
		{
			std::cout << "read ok" << std::endl;
			if (serverConfigParser(server, configString))
			{
				server.printConfig();
				if (server.initService())
					server.run();
				else
				{
					std::cout << "server init fail"<<std::endl;
					g_error = 1;
				}
			}
			else
			{
				std::cout << "server parse not ok"<<std::endl;
				g_error =1;
			}
		}
		else
		{
			std::cout << "read file error" << std::endl;
			g_error =1;
		}
	}
	else
	{
		std::cout << "too many argument"<<std::endl;
		g_error = 1;
	}
	return g_error;
}
