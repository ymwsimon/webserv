/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   client.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mayeung <mayeung@student.42london.com>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/12 19:35:04 by mayeung           #+#    #+#             */
/*   Updated: 2025/11/22 02:05:33 by mayeung          ###   ########.fr       */
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
		void					processData();
		Bytes::const_iterator	&searchForNewLine(Bytes::const_iterator &it);
		Service					&service;
		Client();
	public:
		Client(Service &ser);
		~Client();
		int							sendData(struct epoll_event *evt);
		int 						recvData(struct epoll_event *evt);
		const Bytes					&getIncomingData() const;
		const std::deque<Request>	&getRequests() const;
		const std::deque<Response>	&getResponses() const;
		Client						&operator=(const Client &right);
};
