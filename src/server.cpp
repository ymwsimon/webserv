/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   server.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mayeung <mayeung@student.42london.com>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/14 19:25:58 by mayeung           #+#    #+#             */
/*   Updated: 2026/02/12 19:19:32 by mayeung          ###   ########.fr       */
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
	epollFd = epoll_create(200);
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
		clientsConnection = right.clientsConnection;
		cgiPipeFd = right.cgiPipeFd;
		epollFd = right.epollFd;
	}
	return *this;
}

void	Server::run()
{
	struct epoll_event 	incomingEvt[500];
	struct epoll_event 	evt;
	int					numFd;
	Bytes				data;

	while (true)
	{
		numFd = epoll_wait(epollFd, incomingEvt, 500, -1);
		if (numFd < 0)
			std::cerr<<"epoll wait error"<< std::endl;
		int	n = 0;
		for (int i = 0; i < numFd; ++i)
		{
			evt = incomingEvt[i];
			if (evt.events & EPOLLIN)
			{
				// std::cout << "fd for epollin: " << evt.data.fd << std::endl;
				if (cgiPipeFd.count(evt.data.fd) > 0)
				{
					// std::cout << "extract data from pipe fd: " << evt.data.fd << std::endl;
					// std::cout<<"extract from pipe"<<std::endl;
					cgiPipeFd[evt.data.fd]->processResponseCgi(EXTRACT_PIPE);
					// std::cout << "finish extract from pipe" << std::endl;
				}
				else if (services.count(evt.data.fd) > 0)
				{
					std::cout << "incoming fd " << evt.data.fd << std::endl;
					if (!epollOperation(evt.data.fd, EPOLL_CTL_ADD, false))
						std::cout << "error accept new socket to epoll" << std::endl;
				}
				else
				{
					// if (clientsConnection.count(evt.data.fd) > 0
					// 	&& clientsConnection.at(evt.data.fd).recvData(&evt) == 0)
					// 	epollOperation(evt.data.fd, EPOLL_CTL_DEL, true);
					if (clientsConnection.count(evt.data.fd) > 0)
						clientsConnection.at(evt.data.fd).recvData(&evt);
				}
			}
			else if (evt.events & EPOLLOUT)
			{
				++n;
				// std::cout<<"epollout:"<<evt.data.fd<<std::endl;
				// if (clientsConnection.count(evt.data.fd) > 0
				// 	&& !clientsConnection.at(evt.data.fd).sendData(&evt))
				// 	{}
					// epollOperation(evt.data.fd, EPOLL_CTL_DEL, true);
				if (clientsConnection.count(evt.data.fd) > 0)
					clientsConnection.at(evt.data.fd).sendData(&evt);
				if (cgiPipeFd.count(evt.data.fd) > 0)
				{
					if (cgiPipeFd[evt.data.fd]->getResponses().front().needCloseCgiInFd())
					{
						std::cout << "close cgi in pipe fd:"<<cgiPipeFd[evt.data.fd]->getResponses().front().getCgiInFd()<<std::endl;
						epollOperation(evt.data.fd, EPOLL_CTL_DEL, false);
					}
					else
						cgiPipeFd[evt.data.fd]->processResponseCgi(WRITE_PIPE);
				}
				if (clientsConnection.count(evt.data.fd) > 0
					&& !clientsConnection.at(evt.data.fd).getResponses().empty()
					&& clientsConnection.at(evt.data.fd).getResponses().front().isAddFdStage()
					&& clientsConnection.at(evt.data.fd).getResponses().front().statusOK())
					{
						std::cout<<"add pipe to epoll:" << evt.data.fd<<std::endl;
						epollOperation(evt.data.fd, EPOLL_CTL_ADD, false);
						std::cout<<"add pipe to epoll fin"<<std::endl;
					}
			}
			else if ((evt.events & EPOLLRDHUP) || (evt.events & EPOLLHUP))
				epollOperation(evt.data.fd, EPOLL_CTL_DEL, false);
			else if (evt.events & EPOLLERR)
			{
				std::cout << "error fd:" << evt.data.fd << std::endl;
			}
		}
		// std::cerr<<clientsConnection.size()<<std::endl;
	}
}

bool	Server::epollOperation(int fd, int op, bool needToStop)
{
	struct epoll_event 	newEvt;

	if (op == EPOLL_CTL_ADD)
	{
		if (clientsConnection.count(fd) == 0)
		{
			struct addrinfo addr = *services.at(fd).getAddrInfo();

			newEvt.data.fd = accept(fd, addr.ai_addr, &addr.ai_addrlen);
			if (newEvt.data.fd < 0)
			{
				std::cout << "error accept new connection" << std::endl;
				return false;
			}
			setToNonBlock(newEvt.data.fd);
			newEvt.events = EPOLLIN | EPOLLOUT | EPOLLHUP | EPOLLRDHUP | EPOLLERR;
			clientsConnection.insert(std::make_pair(newEvt.data.fd, Client(&services.at(fd))));
			if (epoll_ctl(epollFd, op, newEvt.data.fd, &newEvt) < 0)
			{
				std::cout << "error add new connection/pipe to epoll" << std::endl;
				return false;
			}
		}
		else
		{
			newEvt.data.fd = clientsConnection.at(fd).getResponses().front().getCgiOutFd();
			std::cout<<"cgi out fd:"<<newEvt.data.fd<<std::endl;
			newEvt.events = EPOLLIN | EPOLLHUP | EPOLLRDHUP | EPOLLERR;
			cgiPipeFd.insert(std::make_pair(newEvt.data.fd, &clientsConnection.at(fd)));
			if (epoll_ctl(epollFd, op, newEvt.data.fd, &newEvt) < 0)
			{
				std::cout << "error add new connection/pipe to epoll" << std::endl;
				return false;
			}
			if (clientsConnection.at(fd).getRequests().front().isChunkMode())
			{
				newEvt.data.fd = clientsConnection.at(fd).getResponses().front().getCgiInFd();
				std::cout<<"cgi int fd:"<<newEvt.data.fd<<std::endl;
				newEvt.events = EPOLLOUT | EPOLLHUP | EPOLLRDHUP | EPOLLERR;
				cgiPipeFd.insert(std::make_pair(newEvt.data.fd, &clientsConnection.at(fd)));
				if (epoll_ctl(epollFd, op, newEvt.data.fd, &newEvt) < 0)
				{
					std::cout << "error add new connection/pipe to epoll" << std::endl;
					return false;
				}
			}
		}
		// if (epoll_ctl(epollFd, op, newEvt.data.fd, &newEvt) < 0)
		// {
		// 	std::cout << "error add new connection/pipe to epoll" << std::endl;
		// 	return false;
		// }
	}
	else if (op == EPOLL_CTL_DEL)
	{
		if (cgiPipeFd.count(fd) > 0)
		{
			cgiPipeFd.erase(fd);
			newEvt.events = EPOLLIN | EPOLLOUT | EPOLLHUP | EPOLLRDHUP | EPOLLERR;
		}
		else
		{
			if (clientsConnection.count(fd) > 0 && needToStop)
				clientsConnection.at(fd).processResponseCgi(KILL_PROCESS);
			clientsConnection.erase(fd);
			newEvt.events = EPOLLIN | EPOLLOUT | EPOLLHUP | EPOLLRDHUP | EPOLLERR;
		}
		newEvt.data.fd = fd;
		if (epoll_ctl(epollFd, op, fd, &newEvt) < 0)
		{
			std::cout << "delete fd " << fd << " from epoll fail" << std::endl;
			return false;
		}
		if (close(fd) < 0)
		{
			std::cout << "close fd " << fd << " fail" << std::endl;
			return false;
		}
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

const std::map<int, Client>	&Server::getClients() const
{
	return clientsConnection;
}

const int	&Server::getEpollFd() const
{
	return epollFd;
}
