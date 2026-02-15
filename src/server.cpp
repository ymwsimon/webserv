/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   server.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mayeung <mayeung@student.42london.com>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/14 19:25:58 by mayeung           #+#    #+#             */
/*   Updated: 2026/02/15 19:52:51 by mayeung          ###   ########.fr       */
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
		numFd = epoll_wait(epollFd, incomingEvt, 500, 500);
		if (numFd < 0)
			std::cerr<<"epoll wait error"<< std::endl;
		// int	n = 0;
		for (int i = 0; i < numFd; ++i)
		{
			evt = incomingEvt[i];
			if (evt.events & EPOLLIN)
			{
				if (cgiPipeFd.count(evt.data.fd) > 0)
				{
					cgiPipeFd.at(evt.data.fd)->processResponseCgi(EXTRACT_PIPE);
					if (evt.data.fd != cgiPipeFd.at(evt.data.fd)->getResponseCgiOutPipeFd())
						std::cerr<<"what2"<< evt.data.fd <<std::endl;
					// if (cgiPipeFd.at(evt.data.fd)->timeToRemoveCgiPipeFromEpoll())
					// {
					// 	std::cout<<"time to remove cgi out fd from epoll:"<<evt.data.fd<<std::endl;
					// 	epollOperation(evt.data.fd, EPOLL_CTL_DEL, false);
					// }
				}
				else if (services.count(evt.data.fd) > 0)
				{
					std::cout << "incoming fd:" << evt.data.fd << std::endl;
					if (!epollOperation(evt.data.fd, EPOLL_CTL_ADD, false))
						std::cout << "error accept new socket to epoll" << std::endl;
				}
				else if (clientsConnection.count(evt.data.fd) > 0)
				{
					if (clientsConnection.at(evt.data.fd).recvData(&evt) == 0)
					{
						std::cout<<"time to close client fd:"<<evt.data.fd<<std::endl;
						// epollOperation(evt.data.fd, EPOLL_CTL_DEL, false);
					}
				}
			}
			if (evt.events & EPOLLOUT)
			{
				if (clientsConnection.count(evt.data.fd) > 0)
				{
					// std::cout<<"client with fd may send start:"<< evt.data.fd<<std::endl;
					// if (clientsConnection.at(evt.data.fd).getResponses().size()
					// 	&& clientsConnection.at(evt.data.fd).getResponses().front().getResultPage().size()
					// 	&& clientsConnection.at(evt.data.fd).getResponses().front().isChunkMode()
					// 	&& clientsConnection.at(evt.data.fd).getResponses().front().isCGI())
					// {
					// 	std::cout<<"something in result page:"<<evt.data.fd <<std::endl;
					// }
					clientsConnection.at(evt.data.fd).sendData(&evt);
					if (evt.data.fd != clientsConnection.at(evt.data.fd).getSocketFd())
						std::cerr<<"what:"<< evt.data.fd<<std::endl;
					// std::cout<<"client with fd may send end:"<< evt.data.fd <<std::endl;
					// if (clientsConnection.at(evt.data.fd).timeToAddCgiPipeToEpoll())
					// {
					// 	std::cout<<"client fd add cgi pipe fd to epoll:" << evt.data.fd<<std::endl;
					// 	epollOperation(evt.data.fd, EPOLL_CTL_ADD, false);
					// 	std::cout<<"client fd add cgi pipe fd to epoll fin"<<std::endl;
					// }
				}
				else if (cgiPipeFd.count(evt.data.fd) > 0)
				{
					cgiPipeFd.at(evt.data.fd)->processResponseCgi(WRITE_PIPE);
					if (evt.data.fd != cgiPipeFd.at(evt.data.fd)->getResponseCgiInPipeFd())
						std::cerr<<"what3" << evt.data.fd<<std::endl;
					// if (cgiPipeFd[evt.data.fd]->timeToCloseCgiInPipe())
					// {
					// 	std::cout << "close cgi in pipe fd:"
					// 		<< cgiPipeFd[evt.data.fd]->getResponses().front().getCgiInFd()
					// 		<< std::endl;
					// 	epollOperation(evt.data.fd, EPOLL_CTL_DEL, false);
					// }
				}
			}
			if ((evt.events & EPOLLRDHUP) || (evt.events & EPOLLHUP))
			{
				if (clientsConnection.count(evt.data.fd) > 0)
				{
					std::cout<<"fd from client closed:"<<evt.data.fd<<std::endl;
					// if (clientsConnection[evt.data.fd].)
					// if (cgiPipeFd.count(clientsConnection[evt.data.fd].))
					epollOperation(evt.data.fd, EPOLL_CTL_DEL, true);
				}
				else
				{
					if (cgiPipeFd.at(evt.data.fd)->getResponseCgiOutPipeFd() == evt.data.fd)
						cgiPipeFd.at(evt.data.fd)->processResponseCgi(EXTRACT_PIPE);
					if (cgiPipeFd.at(evt.data.fd)->getResponseCgiOutPipeFd() != evt.data.fd ||
						cgiPipeFd.at(evt.data.fd)->timeToRemoveCgiPipeFromEpoll())
					{
						if (cgiPipeFd.at(evt.data.fd)->getResponseCgiOutPipeFd() == evt.data.fd)
							std::cout<<"cgiout fd hungup:"<<evt.data.fd<<std::endl;
						else
							std::cout<<"cgiin fd hungup:"<<evt.data.fd<<std::endl;
						epollOperation(evt.data.fd, EPOLL_CTL_DEL, false);
					}
				}
			}
			if (evt.events & EPOLLERR)
			{
				std::cout << "error fd:" << evt.data.fd << std::endl;
			}
		}
		// std::cerr<<"n client conn:"<<clientsConnection.size()<<std::endl;
		// std::cerr<<"n cgi pipe:"<<cgiPipeFd.size()<<std::endl;
		// std::cerr<<"n epoll fd:"<<fdInEpoll.size()<<std::endl;

		std::vector<int>	toDelete;
		for (std::map<int, Client*>::iterator it = cgiPipeFd.begin(); it != cgiPipeFd.end(); ++it)
		{
			if (it->first == it->second->getResponseCgiInPipeFd()
				&& it->second->timeToCloseCgiInPipe())
			{
				std::cout << "time to close cgi in pipe fd:"
					<< it->second->getResponses().front().getCgiInFd()
					<< std::endl;
					// epollOperation(it->first, EPOLL_CTL_DEL, false);
				toDelete.push_back(it->first);
			}
			if (it->first == it->second->getResponseCgiOutPipeFd()
				&& it->second->timeToRemoveCgiPipeFromEpoll())
			{
				std::cout<<"time to remove cgi out fd from epoll:"
					<<it->second->getResponses().front().getCgiOutFd()<<std::endl;
				// epollOperation(it->first, EPOLL_CTL_DEL, false);
				toDelete.push_back(it->first);
			}
		}
		for (std::vector<int>::iterator it = toDelete.begin(); it != toDelete.end(); ++it)
			epollOperation(*it, EPOLL_CTL_DEL, false);

		for (std::map<int, Client>::iterator it = clientsConnection.begin(); it != clientsConnection.end(); ++it)
		{
			it->second.processRequestResponse();
			// if (cgiPipeFd.count(it->second.getResponseCgiInPipeFd()) == 0
			// 	&& cgiPipeFd.count(it->second.getResponseCgiOutPipeFd()) == 0
			// 	&& it->second.isOkToRemoveRequestResponse())
			if (it->second.isOkToRemoveRequestResponse())
			{
				it->second.removeReqResPair();
			}
			if (it->second.timeToAddCgiPipeToEpoll())
			{
				std::cout<<"client fd add cgi pipe fd to epoll:" << it->first<<std::endl;
				epollOperation(it->first, EPOLL_CTL_ADD, false);
				it->second.setCgiStageToWaiting();
				std::cout<<"client fd add cgi pipe fd to epoll fin"<<std::endl;
			}
		}
	}
}

bool	Server::epollOperation(int fd, int op, bool needToStop)
{
	struct epoll_event 	newEvt;

	if (op == EPOLL_CTL_ADD)
	{
		if (services.count(fd) > 0)
		{
			struct addrinfo addr = *services.at(fd).getAddrInfo();

			newEvt.data.fd = accept(fd, addr.ai_addr, &addr.ai_addrlen);
			if (newEvt.data.fd < 0)
			{
				std::cout << "error accept new connection" << std::endl;
				return false;
			}
			// setToNonBlock(newEvt.data.fd);
			std::cout<<"new client fd:"<<newEvt.data.fd<<std::endl;
			newEvt.events = EPOLLIN | EPOLLOUT | EPOLLHUP | EPOLLRDHUP | EPOLLERR;
			clientsConnection.insert(std::make_pair(newEvt.data.fd, Client(&services.at(fd), newEvt.data.fd)));
			if (fdInEpoll.count(newEvt.data.fd) > 0)
				std::cerr<<"already in epoll fd:"<<newEvt.data.fd<<std::endl;
			fdInEpoll.insert(newEvt.data.fd);
			if (epoll_ctl(epollFd, op, newEvt.data.fd, &newEvt) < 0)
			{
				std::cout << "error add new connection/pipe to epoll" << std::endl;
				return false;
			}
		}
		else if (clientsConnection.count(fd) > 0)
		{
			newEvt.data.fd = clientsConnection.at(fd).getResponses().front().getCgiOutFd();
			std::cout<<"new cgi out fd to epoll:"<<newEvt.data.fd<<std::endl;
			newEvt.events = EPOLLIN | EPOLLHUP | EPOLLRDHUP | EPOLLERR;
			cgiPipeFd.insert(std::make_pair(newEvt.data.fd, &clientsConnection.at(fd)));
			if (fdInEpoll.count(newEvt.data.fd) > 0)
				std::cerr<<"already in epoll fd:"<<newEvt.data.fd<<std::endl;
			fdInEpoll.insert(newEvt.data.fd);
			if (epoll_ctl(epollFd, op, newEvt.data.fd, &newEvt) < 0)
			{
				std::cout << "error add new connection/pipe to epoll" << std::endl;
				return false;
			}
			if (clientsConnection.at(fd).getRequests().front().isChunkMode())
			{
				newEvt.data.fd = clientsConnection.at(fd).getResponses().front().getCgiInFd();
				std::cout<<"new cgi int fd to epoll:"<<newEvt.data.fd<<std::endl;
				newEvt.events = EPOLLOUT | EPOLLHUP | EPOLLRDHUP | EPOLLERR;
				cgiPipeFd.insert(std::make_pair(newEvt.data.fd, &clientsConnection.at(fd)));
				if (fdInEpoll.count(newEvt.data.fd) > 0)
					std::cerr<<"already in epoll fd:"<<newEvt.data.fd<<std::endl;
				fdInEpoll.insert(newEvt.data.fd);
				if (epoll_ctl(epollFd, op, newEvt.data.fd, &newEvt) < 0)
				{
					std::cout << "error add new connection/pipe to epoll" << std::endl;
					return false;
				}
			}
		}
		else
			std::cerr<<"?!?!2"<<std::endl;
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
			if (cgiPipeFd.at(fd)->getResponseCgiInPipeFd() == fd)
				newEvt.events = EPOLLOUT | EPOLLHUP | EPOLLRDHUP | EPOLLERR;
			else
				newEvt.events = EPOLLIN | EPOLLHUP | EPOLLRDHUP | EPOLLERR;
			cgiPipeFd.erase(fd);
			std::cout<<"pipe fd removed from epoll:"<<fd<<std::endl;
		}
		else if (clientsConnection.count(fd) > 0)
		{
			if (clientsConnection.count(fd) > 0 && needToStop)
				clientsConnection.at(fd).processResponseCgi(KILL_PROCESS);
			clientsConnection.erase(fd);
			std::cout<<"client fd removed from epoll:"<<fd<<std::endl;
			newEvt.events = EPOLLIN | EPOLLOUT | EPOLLHUP | EPOLLRDHUP | EPOLLERR;
		}
		else
			std::cerr<<"?!?!"<<std::endl;
		newEvt.data.fd = fd;
		if (fdInEpoll.count(newEvt.data.fd) == 0)
			std::cerr<<"not in epoll fd:"<<newEvt.data.fd<<std::endl;
		fdInEpoll.erase(newEvt.data.fd);
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
