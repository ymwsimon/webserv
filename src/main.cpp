/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mayeung <mayeung@student.42london.com>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/05 19:29:27 by mayeung           #+#    #+#             */
/*   Updated: 2026/02/11 16:16:56 by mayeung          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../include/server.hpp"
#include "../include/utils.hpp"

void	signalHandler(int sig)
{
	if (sig == SIGINT)
		std::exit(0);
}

int	main(int argc, char **argv)
{
	Server	server;

	signal(SIGINT, signalHandler);
	server.run();
	(void)argc;
	(void)argv;
	return 0;
}
