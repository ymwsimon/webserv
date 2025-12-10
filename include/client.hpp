/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   client.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mayeung <mayeung@student.42london.com>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/12 19:35:04 by mayeung           #+#    #+#             */
/*   Updated: 2025/12/10 17:53:10 by mayeung          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once
#include "../include/utils.hpp"
#include "../include/service.hpp"
#include "../include/request.hpp"
#include "../include/response.hpp"
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
		Service					&service;
		Client();
		void					processData();
		Bytes::const_iterator	&searchForNewLine(Bytes::const_iterator &it);
	public:
		Client(Service &ser);
		Client(const Client &right);
		~Client();
		Client						&operator=(const Client &right);
		int							sendData(struct epoll_event *evt);
		int 						recvData(struct epoll_event *evt);
		const Bytes					&getIncomingData() const;
		const std::deque<Request>	&getRequests() const;
		const std::deque<Response>	&getResponses() const;
};
