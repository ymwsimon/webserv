/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   service.hpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mayeung <mayeung@student.42london.com>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/13 21:58:23 by mayeung           #+#    #+#             */
/*   Updated: 2025/11/19 00:01:05 by mayeung          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once
#include <sys/socket.h>
#include "config.hpp"
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
		~Service();
		struct addrinfo	*getAddrInfo();
		const u_int32_t	&getAddrLen() const;
		const int		&getSocketFd() const;
		const Config	&getServiceConfig() const;
		Service			&operator=(const Service &right);
};
