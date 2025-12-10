/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   service.hpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mayeung <mayeung@student.42london.com>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/13 21:58:23 by mayeung           #+#    #+#             */
/*   Updated: 2025/12/10 17:55:26 by mayeung          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once
#include <sys/socket.h>
#include "config.hpp"
#include "request.hpp"
#include <netdb.h>
#include <fcntl.h>
#include <sys/types.h>
#include <cstring>
#include <iostream>

class Service
{
	private:
		struct addrinfo		*addrInfo;
		u_int32_t			addrLen;
		int					socketFd;
		Config				serviceConfig;
		Service();
	public:
		Service(Config config);
		Service(const Service &right);
		~Service();
		Service			&operator=(const Service &right);
		struct addrinfo	*getAddrInfo() const;
		const u_int32_t	&getAddrLen() const;
		const int		&getSocketFd() const;
		const Config	&getServiceConfig() const;
		const Location	*findMatchingRoute(const Request &req) const;
};
