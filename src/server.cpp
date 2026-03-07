/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   server.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mayeung <mayeung@student.42london.com>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/14 19:25:58 by mayeung           #+#    #+#             */
/*   Updated: 2026/03/05 12:35:34 by mayeung          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../include/server.hpp"

Server::Server()
{
	epollFd = -1;
}

Server::Server(const Server &right)
{
	*this = right;
}

Server::~Server()
{
	socketServices.clear();
	services.clear();
	configs.clear();
	cgiPipeFd.clear();
	clientsConnection.clear();
	if (epollFd != -1)
		close(epollFd);
}

Server	&Server::operator=(const Server &right)
{
	if (this != &right)
	{
		configs = right.configs;
		socketServices = right.socketServices;
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
	int					numEvent;
	Bytes				data;

	while (!g_finish && !g_error)
	{
		numEvent = epoll_wait(epollFd, incomingEvt, 500, 100);
		if (numEvent < 0 && !g_finish)
		{
			g_error = 1;
			std::cerr<<"epoll wait error"<< std::endl;
		}
		for (int i = 0; i < numEvent; ++i)
		{
			evt = incomingEvt[i];
			if (evt.events & EPOLLIN)
			{
				if (cgiPipeFd.count(evt.data.fd) > 0)
				{
					cgiPipeFd.at(evt.data.fd)->processResponseCgi(EXTRACT_PIPE);
				}
				else if (socketServices.count(evt.data.fd) > 0)
				{
					if (!epollOperation(evt.data.fd, EPOLL_CTL_ADD, false))
						std::cout << "error accept new socket to epoll" << std::endl;
				}
				else if (clientsConnection.count(evt.data.fd) > 0)
				{
					clientsConnection.at(evt.data.fd).recvData(&evt);
				}
			}
			if (evt.events & EPOLLOUT)
			{
				if (clientsConnection.count(evt.data.fd) > 0)
				{
					clientsConnection.at(evt.data.fd).sendData(&evt);
				}
				else if (cgiPipeFd.count(evt.data.fd) > 0)
				{
					cgiPipeFd.at(evt.data.fd)->processResponseCgi(WRITE_PIPE);
				}
			}
			if ((evt.events & EPOLLRDHUP) || (evt.events & EPOLLHUP))
			{
				if (clientsConnection.count(evt.data.fd) > 0)
				{
					Client	&c = clientsConnection.at(evt.data.fd);

					if (cgiPipeFd.count(c.getResponseCgiInPipeFd()))
						epollOperation(c.getResponseCgiInPipeFd(), EPOLL_CTL_DEL, false);
					if (cgiPipeFd.count(c.getResponseCgiOutPipeFd()))
						epollOperation(c.getResponseCgiOutPipeFd(), EPOLL_CTL_DEL, false);
					epollOperation(evt.data.fd, EPOLL_CTL_DEL, true);
				}
				else
				{
					if (cgiPipeFd.at(evt.data.fd)->getResponseCgiOutPipeFd() == evt.data.fd)
					{
						cgiPipeFd.at(evt.data.fd)->processResponseCgi(EXTRACT_PIPE);
					}
					if (cgiPipeFd.at(evt.data.fd)->getResponseCgiOutPipeFd() != evt.data.fd ||
						cgiPipeFd.at(evt.data.fd)->timeToRemoveCgiPipeFromEpoll())
					{
						epollOperation(evt.data.fd, EPOLL_CTL_DEL, false);
					}
				}
			}
			if (evt.events & EPOLLERR)
			{
				if (cgiPipeFd.count(evt.data.fd) > 0)
					epollOperation(evt.data.fd, EPOLL_CTL_DEL, false);
				else if (clientsConnection.count(evt.data.fd) > 0)
				{
					Client	&c = clientsConnection.at(evt.data.fd);

					if (cgiPipeFd.count(c.getResponseCgiInPipeFd()))
						epollOperation(c.getResponseCgiInPipeFd(), EPOLL_CTL_DEL, false);
					if (cgiPipeFd.count(c.getResponseCgiOutPipeFd()))
						epollOperation(c.getResponseCgiOutPipeFd(), EPOLL_CTL_DEL, false);
					epollOperation(evt.data.fd, EPOLL_CTL_DEL, true);
				}
			}
		}
		std::vector<int>	toDelete;
		for (std::map<int, Client*>::iterator it = cgiPipeFd.begin(); it != cgiPipeFd.end(); ++it)
		{
			if ((it->first == it->second->getResponseCgiInPipeFd()
				&& it->second->timeToCloseCgiInPipe()) || it->second->getClientError())
			{
				toDelete.push_back(it->first);
			}
			if ((it->first == it->second->getResponseCgiOutPipeFd()
				&& it->second->timeToRemoveCgiPipeFromEpoll()) || it->second->getClientError())
			{
				toDelete.push_back(it->first);
			}
		}
		for (std::vector<int>::iterator it = toDelete.begin(); it != toDelete.end(); ++it)
			epollOperation(*it, EPOLL_CTL_DEL, false);

		for (std::map<int, Client>::iterator it = clientsConnection.begin(); it != clientsConnection.end(); ++it)
		{
			if (it->second.getClientError())
			{
				Client	&c = it->second;

				if (cgiPipeFd.count(c.getResponseCgiInPipeFd()))
					epollOperation(c.getResponseCgiInPipeFd(), EPOLL_CTL_DEL, false);
				if (cgiPipeFd.count(c.getResponseCgiOutPipeFd()))
					epollOperation(c.getResponseCgiOutPipeFd(), EPOLL_CTL_DEL, false);
				epollOperation(it->first, EPOLL_CTL_DEL, true);
			}
			else
			{
				it->second.processRequestResponse();
				if (it->second.isOkToRemoveRequestResponse())
				{
					it->second.removeReqResPair();
				}
				if (it->second.timeToAddCgiPipeToEpoll())
				{
					epollOperation(it->first, EPOLL_CTL_ADD, false);
					it->second.setCgiStageToWaiting();
				}
			}
		}
	}
}

bool	Server::epollOperation(int fd, int op, bool needToStop)
{
	struct epoll_event 	newEvt;

	if (op == EPOLL_CTL_ADD)
	{
		if (socketServices.count(fd) > 0)
		{
			struct addrinfo addr = *socketServices.at(fd)->getAddrInfo();

			newEvt.data.fd = accept(fd, addr.ai_addr, &addr.ai_addrlen);
			if (newEvt.data.fd < 0)
			{
				std::cout << "error accept new connection" << std::endl;
				return false;
			}
			newEvt.events = EPOLLIN | EPOLLOUT | EPOLLHUP | EPOLLRDHUP | EPOLLERR;
			clientsConnection.insert(std::make_pair(newEvt.data.fd, Client(socketServices.at(fd), newEvt.data.fd)));
			if (epoll_ctl(epollFd, op, newEvt.data.fd, &newEvt) < 0)
			{
				std::cout << "error add new connection/pipe to epoll" << std::endl;
				return false;
			}
		}
		else if (clientsConnection.count(fd) > 0)
		{
			newEvt.data.fd = clientsConnection.at(fd).getResponses().front().getCgiOutFd();
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
				newEvt.events = EPOLLOUT | EPOLLHUP | EPOLLRDHUP | EPOLLERR;
				cgiPipeFd.insert(std::make_pair(newEvt.data.fd, &clientsConnection.at(fd)));
				if (epoll_ctl(epollFd, op, newEvt.data.fd, &newEvt) < 0)
				{
					std::cout << "error add new connection/pipe to epoll" << std::endl;
					return false;
				}
			}
		}
	}
	else if (op == EPOLL_CTL_DEL)
	{
		if (cgiPipeFd.count(fd) > 0)
		{
			if (cgiPipeFd.at(fd)->getResponseCgiInPipeFd() == fd)
				newEvt.events = EPOLLOUT | EPOLLHUP | EPOLLRDHUP | EPOLLERR;
			else
				newEvt.events = EPOLLIN | EPOLLHUP | EPOLLRDHUP | EPOLLERR;
			cgiPipeFd.erase(fd);
		}
		else if (clientsConnection.count(fd) > 0)
		{
			if (needToStop)
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

const std::map<int, Service*>	&Server::getServices() const
{
	return socketServices;
}

const std::map<int, Client>	&Server::getClients() const
{
	return clientsConnection;
}

const int	&Server::getEpollFd() const
{
	return epollFd;
}

void	Server::addConfig(Config &c)
{
	configs.push_back(c);
}


void	Server::printConfig()
{
	for (size_t i = 0; i < configs.size(); ++i)
		configs.at(i).printConfig();
}

bool	Server::initService()
{
	struct epoll_event 	evt;

	epollFd = epoll_create(1);
	if (epollFd < 0)
	{
		g_error = 1;
		std::cout << "error create epoll" << std::endl;
		return false;
	}
	services.reserve(configs.size());
	for (size_t i = 0; i < configs.size(); ++i)
	{
		services.push_back(Service(configs.at(i)));
		if (!services.at(i).getInitOk())
		{
			services.clear();
			return false;
		}
		socketServices.insert(std::make_pair(services.at(i).getSocketFd(), &services.at(i)));
		evt.data.fd = services.at(i).getSocketFd();
		evt.events = EPOLLIN | EPOLLOUT;
		if (epoll_ctl(epollFd, EPOLL_CTL_ADD, evt.data.fd, &evt) < 0)
		{
			g_error = 1;
			std::cout << "error add socket to epoll" << std::endl;
			return false;
		}
	}
	return true;
}
