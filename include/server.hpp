/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   server.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mayeung <mayeung@student.42london.com>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/13 17:31:55 by mayeung           #+#    #+#             */
/*   Updated: 2026/01/24 22:32:01 by mayeung          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once
#include "config.hpp"
#include "client.hpp"
#include "service.hpp"
#include <sys/epoll.h>
#include <vector>
#include <map>
#include <set>
#include <algorithm>

class Server
{
	private:
		std::vector<Config>		configs;
		std::map<int, Service>	services;
		std::set<Client>		clients;
		std::map<int, Client>	clientsConnection;
		std::map<int, Client *>	cgiPipeFd;
		int						epollFd;
		bool					addNewConn(struct epoll_event evt, struct addrinfo addr);
		bool					epollOperation(struct epoll_event evt, int op);
	public:
		Server();
		Server(const Server &right);
		~Server();
		Server							&operator=(const Server &right);
		void							run();
		const std::vector<Config>		&getConfigs() const;
		const std::map<int, Service>	&getServices() const;
		const std::map<int, Client>		&getClients() const;
		const int						&getEpollFd() const;
};
