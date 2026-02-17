/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mayeung <mayeung@student.42london.com>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/05 19:29:27 by mayeung           #+#    #+#             */
/*   Updated: 2026/02/17 11:41:17 by mayeung          ###   ########.fr       */
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

	signal(SIGINT, signalHandler);
	server.run();
	(void)argc;
	(void)argv;
	return g_error;
}
