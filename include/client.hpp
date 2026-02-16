/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   client.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mayeung <mayeung@student.42london.com>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/12 19:35:04 by mayeung           #+#    #+#             */
/*   Updated: 2026/02/16 11:06:42 by mayeung          ###   ########.fr       */
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
		int						socketFd;
		Byte					buf[TRANSFER_SIZE];
		Client();
		void					processRequest();
		void					processResponse();
		void					writeIncomingData(int readSize);
		void					writeOutData(size_t size);
		Bytes::const_iterator	&searchForNewLine(Bytes::const_iterator &it);
	public:
		Client(Service *ser, int fd);
		Client(const Client &right);
		~Client();
		Client						&operator=(const Client &right);
		int							sendData(struct epoll_event *evt);
		int 						recvData(struct epoll_event *evt);
		void						processResponseCgi(int op);
		bool						timeToCloseCgiInPipe() const;
		bool						timeToAddCgiPipeToEpoll() const;
		bool						timeToRemoveCgiPipeFromEpoll() const;
		bool						isOkToSendData() const;
		bool						isOkToRemoveRequestResponse() const;
		const Bytes					&getIncomingData() const;
		const std::deque<Request>	&getRequests() const;
		const std::deque<Response>	&getResponses() const;
		int							getResponseCgiInPipeFd() const;
		int							getResponseCgiOutPipeFd() const;
		int							getSocketFd() const;
		void						removeReqResPair();
		void						processRequestResponse();
		void						setCgiStageToWaiting();
};
