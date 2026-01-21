/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   server.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mayeung <mayeung@student.42london.com>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/14 19:25:58 by mayeung           #+#    #+#             */
/*   Updated: 2026/01/21 17:23:10 by mayeung          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../include/server.hpp"

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

Server::Server(const Server &right)
{
	*this = right;
}

Server::~Server()
{

}

Server	&Server::operator=(const Server &right)
{
	if (this != &right)
	{
		configs = right.configs;
		services = right.services;
		clients = right.clients;
		cgiPipeFd = right.cgiPipeFd;
		epollFd = right.epollFd;
	}
	return *this;
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
				std::cout << "fd for epollin: " << evt.data.fd << std::endl;
				if (cgiPipeFd.count(evt.data.fd) > 0)
				{
					std::cout << "extract data from pipe fd: " << evt.data.fd << std::endl;
					cgiPipeFd[evt.data.fd]->processResponseCgi(EXTRACT_PIPE);
				}
				else if (clients.count(evt.data.fd) == 0)
				{
					std::cout << "incoming fd " << evt.data.fd << std::endl;
					if (!addNewConn(evt, *services.at(evt.data.fd).getAddrInfo()))
						std::cout << "error accept new socket to epoll" << std::endl;
				}
				else
				{
					if (clients[evt.data.fd]->recvData(&evt) == 0)
					{
						std::cout << "time to close client fd: " << evt.data.fd << std::endl;
						struct epoll_event 	newEvt;

						delete clients[evt.data.fd];
						clients.erase(evt.data.fd);
						newEvt.data.fd = evt.data.fd;
						newEvt.events = EPOLLIN | EPOLLOUT | EPOLLHUP | EPOLLRDHUP | EPOLLERR;
						if (epoll_ctl(epollFd, EPOLL_CTL_DEL, evt.data.fd, &newEvt) < 0)
							std::cout << "delete client fd " << evt.data.fd << " from epoll fail\n";
						if (close(evt.data.fd) < 0)
							std::cout << "close client fd " << evt.data.fd << " fail\n";
					}
				}
			}
			else if (evt.events & EPOLLOUT)
			{
				clients[evt.data.fd]->sendData(&evt);
				if (clients[evt.data.fd]->getResponses().front().isAddFdStage())
				{
					struct epoll_event 	newEvt;
					const Response		&response = clients[evt.data.fd]->getResponses().front();

					newEvt.data.fd = response.getCgiResFd();
					newEvt.events = EPOLLIN | EPOLLHUP | EPOLLRDHUP | EPOLLERR;
					std::cout << "pipe id to add to epoll:" << newEvt.data.fd << std::endl;
					if (epoll_ctl(epollFd, EPOLL_CTL_ADD, newEvt.data.fd, &newEvt) < 0)
						std::cout << "add pipe to epoll fail\n";
					cgiPipeFd.insert(std::make_pair(newEvt.data.fd, clients[evt.data.fd]));
					clients[evt.data.fd]->processResponseCgi(PROCESS_DATA);
				}
			}
			else if ((evt.events & EPOLLRDHUP) || (evt.events & EPOLLHUP))
			{
				std::cout << "fd for epollhup: " << evt.data.fd << std::endl;
				if (cgiPipeFd.count(evt.data.fd) > 0)
				{
					cgiPipeFd[evt.data.fd]->processResponseCgi(CLOSE_PIPE);
					std::cout << "time to close pipe fd: " << evt.data.fd << std::endl;
					struct epoll_event 	newEvt;

					cgiPipeFd.erase(evt.data.fd);
					newEvt.data.fd = evt.data.fd;
					newEvt.events = EPOLLIN | EPOLLHUP | EPOLLRDHUP | EPOLLERR;
					if (epoll_ctl(epollFd, EPOLL_CTL_DEL, evt.data.fd, &newEvt) < 0)
						std::cout << "delete pipe fd " << evt.data.fd << " from epoll fail.\n";
					if (close(evt.data.fd) < 0)
						std::cout << "close pipe fd " << evt.data.fd << " fail\n";
				}
				else
				{
					struct epoll_event 	newEvt;

					delete clients[evt.data.fd];
					clients.erase(evt.data.fd);
					newEvt.data.fd = evt.data.fd;
					newEvt.events = EPOLLIN | EPOLLOUT | EPOLLHUP | EPOLLRDHUP | EPOLLERR;
					if (epoll_ctl(epollFd, EPOLL_CTL_DEL, evt.data.fd, &newEvt) < 0)
						std::cout << "delete client fd " << evt.data.fd << " from epoll fail\n";
					if (close(evt.data.fd) < 0)
						std::cout << "close client fd " << evt.data.fd << " fail\n";
					std::cout << "connection ended for fd " << evt.data.fd << std::endl;
				}
			}
			else if (evt.events & EPOLLERR)
			{
				std::cout << "error\n";
			}
		}
	}
}

bool Server::addNewConn(struct epoll_event evt, struct addrinfo addr)
{
	struct epoll_event 	newEvt;
	int					newFd;
	Client				*c = new Client(services.at(evt.data.fd));

	newFd = accept(evt.data.fd, addr.ai_addr, &addr.ai_addrlen);
	if (newFd < 0)
	{
		std::cout << "error accept new connection" << std::endl;
		return false;
	}
	std::cout << "new connection fd " << newFd << std::endl;
	clients.insert(std::make_pair(newFd, c));
	newEvt.data.fd = newFd;
	newEvt.events = EPOLLIN | EPOLLOUT | EPOLLHUP | EPOLLRDHUP | EPOLLERR;
	if (epoll_ctl(epollFd, EPOLL_CTL_ADD, newFd, &newEvt) < 0)
	{
		std::cout << "error add new connection to epoll" << std::endl;
		return false;
	}
	return true;
}

const std::vector<Config>	&Server::getConfigs() const
{
	return configs;
}

const std::map<int, Service>	&Server::getServices() const
{
	return services;
}

const std::map<int, Client *>	&Server::getClients() const
{
	return clients;
}

const int	&Server::getEpollFd() const
{
	return epollFd;
}
