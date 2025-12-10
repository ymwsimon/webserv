/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   service.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mayeung <mayeung@student.42london.com>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/14 18:17:40 by mayeung           #+#    #+#             */
/*   Updated: 2025/12/10 17:55:57 by mayeung          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../include/service.hpp"

Service::Service(Config config)
{
	struct addrinfo		addr;
	int					op = 1;

	serviceConfig = config;
	bzero(&addr, sizeof(addr));
	addr.ai_family = AF_INET;
	addr.ai_socktype = SOCK_STREAM;
	addr.ai_flags = AI_PASSIVE;
	addr.ai_protocol = 0;
	if (getaddrinfo("127.0.0.1", "8080", &addr, &addrInfo))
		std::cout << "error get addrinfo" << std::endl;
	socketFd = socket(addrInfo->ai_family, addrInfo->ai_socktype, addrInfo->ai_protocol);
	if (socketFd < 0)
		std::cout << "error create socket" << std::endl;
	std::cout<< "SocketFD " << socketFd << std::endl;
	if (setsockopt(socketFd, SOL_SOCKET, SO_REUSEADDR, &op, sizeof(op)) < 0)
		std::cout << "error set socket op" << std::endl;
	if (bind(socketFd, addrInfo->ai_addr, addrInfo->ai_addrlen) < 0)
		std::cout << "error bind socket" << std::endl;
	if (listen(socketFd, 100) < 0)
		std::cout << "error listen socket" << std::endl;
}

Service::Service(const Service &right)
{
	*this = right;
}

Service::~Service()
{
	
}

Service	&Service::operator=(const Service &right)
{
	if (this != &right)
	{
		addrInfo = right.addrInfo;
		addrLen = right.addrLen;
		socketFd = right.socketFd;
		serviceConfig = right.serviceConfig;
	}
	return *this;
}

struct addrinfo	*Service::getAddrInfo() const
{
	return addrInfo;	
}

const u_int32_t	&Service::getAddrLen() const
{
	return addrLen;
}

const int	&Service::getSocketFd() const
{
	return socketFd;
}

const Config	&Service::getServiceConfig() const
{
	return serviceConfig;
}

const Location	*Service::findMatchingRoute(const Request &req) const
{
	return serviceConfig.getLocationMatch(req.getPaths());
}
