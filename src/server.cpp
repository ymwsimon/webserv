/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   server.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mayeung <mayeung@student.42london.com>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/14 19:25:58 by mayeung           #+#    #+#             */
/*   Updated: 2025/11/16 21:28:34 by mayeung          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../include/server.hpp"

// private:
// 		std::vector<Config>		configs;
// 		std::map<int, Service>	services;
// 		std::map<int, Client>	clients;
// 		int						epollFd;
// 	public:
// 		Server();
// 		~Server();
// 		std::vector<Config>		&getConfigs();
// 		std::map<int, Service>	&getServices();
// 		std::map<int, Client>	&getClients();
// 		int						&getEpollFd();

Server::Server()
{
	struct epoll_event 	evt;
	Config				config;
	Service				s(config);

	configs.push_back(config);
	services.insert(std::make_pair(s.getSocketFd(), s));
	std::cout << "socket fd for service " << s.getSocketFd() << std::endl;
	epollFd = epoll_create(100);
	if (epollFd < 0)
		std::cout << "error create epoll" << std::endl;
	std::cout << "epoll fd " << epollFd << std::endl;
	evt.data.fd = s.getSocketFd();
	evt.events = EPOLLIN | EPOLLOUT;
	if (epoll_ctl(epollFd, EPOLL_CTL_ADD, s.getSocketFd(), &evt) < 0)
		std::cout << "error add socket to epoll" << std::endl;
}

Server::~Server()
{

}

void	Server::run()
{
	struct epoll_event 	incomingEvt[100];
	struct epoll_event 	evt;
	int					numFd;
	Bytes				data;

	while (true)
	{
		numFd = epoll_wait(epollFd, incomingEvt, 100, 1000);
		if (numFd < 0)
			{}//throw error
		for (int i = 0; i < numFd; ++i)
		{
			evt = incomingEvt[i];
			if (evt.events & EPOLLIN)
			{
				if (clients.count(evt.data.fd) == 0)
				{
					std::cout << "incoming fd " << evt.data.fd << std::endl;
					if (!addNewConn(evt, *services.at(evt.data.fd).getAddrInfo()))
						std::cout << "error accept new socket to epoll" << std::endl;
				}
				else
					clients[evt.data.fd].recvData(&evt);
			}
			else if (evt.events & EPOLLOUT)
				clients[evt.data.fd].sendData(&evt);
		}
	}
}

bool Server::addNewConn(struct epoll_event evt, struct addrinfo addr)
{
	struct epoll_event 	newEvt;
	int					newFd;
	Client				c;

	newFd = accept(evt.data.fd, addr.ai_addr, &addr.ai_addrlen);
	if (newFd < 0)
	{
		std::cout << "error accept new connection" << std::endl;
		return false;
	}
	std::cout << "new connection fd " << newFd << std::endl;
	clients[newFd] = c;
	newEvt.data.fd = newFd;
	newEvt.events = EPOLLIN | EPOLLOUT;
	if (epoll_ctl(epollFd, EPOLL_CTL_ADD, newFd, &newEvt) < 0)
	{
		std::cout << "error add new connection to epoll" << std::endl;
		return false;
	}
	return true;
}

const std::vector<Config>	&Server::getConfigs()
{
	return configs;
}

const std::map<int, Service>	&Server::getServices()
{
	return services;
}

const std::map<int, Client>	&Server::getClients()
{
	return clients;
}

const int	&Server::getEpollFd()
{
	return epollFd;
}
