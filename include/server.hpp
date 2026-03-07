/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   server.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mayeung <mayeung@student.42london.com>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/13 17:31:55 by mayeung           #+#    #+#             */
/*   Updated: 2026/03/01 23:26:24 by mayeung          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once
#include "config.hpp"
#include "client.hpp"
#include "service.hpp"
#include "parser.hpp"
#include <sys/epoll.h>
#include <vector>
#include <map>
#include <set>
#include <algorithm>
#include <signal.h>

extern bool	g_finish;
extern int	g_error;

class Server
{
	private:
		std::vector<Config>		configs;
		std::vector<Service>	services;
		std::map<int, Service*>	socketServices;
		std::set<Client>		clients;
		std::map<int, Client>	clientsConnection;
		std::map<int, Client*>	cgiPipeFd;
		int						epollFd;
		bool					epollOperation(int fd, int op, bool needToStop);
	public:
		Server();
		Server(const Server &right);
		~Server();
		Server							&operator=(const Server &right);
		void							run();
		const std::vector<Config>		&getConfigs() const;
		const std::map<int, Service*>	&getServices() const;
		const std::map<int, Client>		&getClients() const;
		const int						&getEpollFd() const;
		void							addConfig(Config &c);
		void							printConfig();
		bool							initService();
};
