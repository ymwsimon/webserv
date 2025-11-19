/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   server.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mayeung <mayeung@student.42london.com>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/13 17:31:55 by mayeung           #+#    #+#             */
/*   Updated: 2025/11/18 21:38:26 by mayeung          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once
#include "config.hpp"
#include "client.hpp"
#include "service.hpp"
#include <sys/epoll.h>
#include <vector>
#include <map>
#include <algorithm>

class Server
{
	private:
		std::vector<Config>		configs;
		std::map<int, Service>	services;
		std::map<int, Client *>	clients;
		int						epollFd;
		bool					addNewConn(struct epoll_event evt, struct addrinfo addr);
	public:
		Server();
		~Server();
		void							run();
		const std::vector<Config>		&getConfigs();
		const std::map<int, Service>	&getServices();
		const std::map<int, Client *>	&getClients();
		const int						&getEpollFd();
};
