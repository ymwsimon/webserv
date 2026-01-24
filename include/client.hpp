/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   client.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mayeung <mayeung@student.42london.com>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/12 19:35:04 by mayeung           #+#    #+#             */
/*   Updated: 2026/01/24 22:54:08 by mayeung          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once
#include "utils.hpp"
#include "service.hpp"
#include "request.hpp"
#include "response.hpp"
#include <string>
#include <iostream>
#include <algorithm>
#include <sys/socket.h>
#include <sys/epoll.h>
#include <deque>
#include <vector>

class Client
{
	private:
		Bytes					incomingData;
		std::deque<Request>		requests;
		std::deque<Response>	responses;
		Service					*service;
		Client();
		void					processData();
		Bytes::const_iterator	&searchForNewLine(Bytes::const_iterator &it);
	public:
		Client(Service *ser);
		Client(const Client &right);
		~Client();
		Client						&operator=(const Client &right);
		int							sendData(struct epoll_event *evt);
		int 						recvData(struct epoll_event *evt);
		void						processResponseCgi(int op);
		const Bytes					&getIncomingData() const;
		const std::deque<Request>	&getRequests() const;
		const std::deque<Response>	&getResponses() const;
};
