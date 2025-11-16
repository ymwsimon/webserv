/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mayeung <mayeung@student.42london.com>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/05 19:29:27 by mayeung           #+#    #+#             */
/*   Updated: 2025/11/16 17:25:48 by mayeung          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <iostream>
#include <sys/epoll.h>
#include <sys/socket.h>
#include <string>
#include <cstring>
#include <vector>
#include <map>
#include <algorithm>
#include <netdb.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sstream>
#include "../include/utils.hpp"
#include "../include/service.hpp"
#include "../include/config.hpp"
#include "../include/server.hpp"

// int	sendData(int eFd, struct epoll_event *evt, Bytes &data)
// {
// 	std::string	str;
// 	std::string	content;
// 	std::stringstream	strStream;

// 	content = "<!DOCTYPE html>"
// 				"<html>"
// 				"<head>"
// 				"<meta content=\"text/html; charset=ISO-8859-1\" http-equiv=\"content-type\">"
// 				"<title>WebServer</title></head>"
// 				"<body>"
// 				"Testing"
// 				"</body></html>";
// 	str = "HTTP/1.1 200 OK\r\n"
// 			"Content-Type: text/html\r\n"
// 			"Content-Length: ";
// 	strStream << content.length();
// 	str += strStream.str();
// 	str += "\r\n\r\n";
// 	str += content;
	
// 	std::vector<unsigned char>::iterator it;
// 	it = std::search(data.begin(), data.end(), DOUBLE_CRLF.begin(), DOUBLE_CRLF.end());
// 	if (it != data.end())
// 	{
// 		std::cout << "request" << std::endl;
// 		for (int i = 0; i < data.size(); ++i)
// 			std::cout << data[i];
// 		std::cout << std::endl;
// 		std::cout << "sending out data" << std::endl;
// 		data = std::vector<unsigned char>(it + 4, data.end());
// 		if (send(evt->data.fd, str.c_str(), str.length(), 0) < 0)
// 			std::cout << "error send data out" << std::endl;
// 	}
// 	// evt->events = EPOLLIN;
// 	// if (epoll_ctl(eFd, EPOLL_CTL_MOD, evt->data.fd, evt) < 0)
// 	// 	std::cout << "error " << std::endl;
// 	return 1;
// }

// int recvData(int eFd, struct epoll_event *evt, Bytes &data)
// {
// 	// std::string	str;
// 	Byte	buf[BUFFER_SIZE];
// 	int		readSize = 0;

// 	// std::cout << "getting data from fd " << evt->data.fd << std::endl;
// 	readSize = recv(evt->data.fd, buf, BUFFER_SIZE, 0);
// 	if (readSize)
// 		std::cout << readSize << std::endl;
// 	if (readSize > 0)
// 	{
// 		// std::cout << readSize << std::endl;
// 		// std::cout << buf <<std::endl;
// 		// buf[readSize] = '\0';
// 		// data += buf;
// 		// Bytes temp(buf);
// 		data.insert(data.end(), buf, buf + readSize);
// 		for (int i = 0; i < data.size(); ++i)
// 			std::cout << data[i];
// 		std::cout << std::endl;
// 		// readSize = recv(evt->data.fd, buf, BUFFER_SIZE, 0);
// 	}
// 	// for (int i = 0; i < data.size(); ++i)
// 	// 	std::cout << data[i];
// 	// std::cout << std::endl;
// 	// std::cout << data.find("\r\n\r\n") << std::endl;
// 	// if (std::search(data.begin(), data.end(), CRLF.begin(), CRLF.end()) != data.end())
// 	// {
// 	// 	// std::cout << "fhweifuh" << std::endl;
// 	// 	evt->events = EPOLLOUT;
// 	// 	if (epoll_ctl(eFd, EPOLL_CTL_MOD, evt->data.fd, evt) < 0)
// 	// 		std::cout << "error " << std::endl;
// 	// }
// 	return readSize;
// }

// bool addNewConn(int eFd, struct epoll_event evt, struct addrinfo &addr, std::vector<int> &clientFdList)
// {
// 	struct epoll_event 	newEvt;
// 	int					newFd;

// 	newFd = accept(evt.data.fd, addr.ai_addr, &addr.ai_addrlen);
// 	if (newFd < 0)
// 	{
// 		std::cout << "error accept new connection" << std::endl;
// 		return false;
// 	}
// 	clientFdList.push_back(newFd);
// 	// std::cout << "the new connection fd " << newFd << std::endl;
// 	// if (fcntl(newFd, F_SETFL, O_NONBLOCK) < 0)
// 	// 	std::cout << "error set socket to non-block" << std::endl;
// 	newEvt.data.fd = newFd;
// 	newEvt.events = EPOLLIN | EPOLLOUT;
// 	if (epoll_ctl(eFd, EPOLL_CTL_ADD, newFd, &newEvt) < 0)
// 	{
// 		std::cout << "error add new connection to epoll" << std::endl;
// 		return false;
// 	}
// 	return true;
// }

int	main(int argc, char **argv)
{
	Server	server;

	server.run();
	(void)argc;
	(void)argv;
	// struct addrinfo		addr;
	// struct addrinfo		*res;
	// u_int32_t			addrLen;
	// int					sFd;
	// int					op = 1;
	// Config				config;
	// Service				service(config);
	// std::vector<int>	clientFd;
	// int					eFd;
	// struct epoll_event 	evt;
	// struct epoll_event 	incomingEvt[100];
	// int					numFd;
	// int					incomingFd;
	// Bytes				data;
	

	// bzero(&addr, sizeof(addr));
	// addr.ai_family = AF_INET;
	// addr.ai_socktype = SOCK_STREAM;
	// addr.ai_flags = AI_PASSIVE;
	// addr.ai_protocol = 0;
	// if (getaddrinfo("127.0.0.1", "8080", &addr, &res))
	// 	std::cout << "error get addrinfo" << std::endl;
	// addrLen = res->ai_addrlen;
	// std::cout<< addrLen << std::endl;
	// sFd = socket(res->ai_family, res->ai_socktype, res->ai_protocol);
	// if (sFd < 0)
	// 	std::cout << "error create socket" << std::endl;
	// std::cout<< sFd << std::endl;
	// if (setsockopt(sFd, SOL_SOCKET, SO_REUSEADDR, &op, sizeof(op)) < 0)
	// 	std::cout << "error set socket op" << std::endl;
	// if (bind(sFd, res->ai_addr, res->ai_addrlen) < 0)
	// 	std::cout << "error bind socket" << std::endl;
	// if (listen(sFd, 100) < 0)
	// 	std::cout << "error listen socket" << std::endl;
	// if (fcntl(sFd, F_SETFL, O_NONBLOCK) < 0)
	// 	std::cout << "error set socket to non-block" << std::endl;
	// eFd = epoll_create(100);
	// if (eFd < 0)
	// 	std::cout << "error create epoll" << std::endl;
	
	// evt.data.fd = sFd;
	// evt.events = EPOLLIN | EPOLLOUT;
	// if (epoll_ctl(eFd, EPOLL_CTL_ADD, sFd, &evt) < 0)
	// 	std::cout << "error add socket to epoll" << std::endl;
	// while (true)
	// {
	// 	numFd = epoll_wait(eFd, incomingEvt, 100, 1000);
	// 	if (numFd > 0)
	// 	{
	// 		for (int i = 0; i < numFd; ++i)
	// 		{
	// 			evt = incomingEvt[i];
	// 			if (evt.events & EPOLLIN)
	// 			{
	// 				// std::cout << "new epollin event" << std::endl;
	// 				if (std::find(clientFd.begin(), clientFd.end(), evt.data.fd) == clientFd.end())
	// 				{
	// 					if (!addNewConn(eFd, evt, addr, clientFd))
	// 						std::cout << "error accept new socket to epoll" << std::endl;
	// 				}
	// 				else
	// 					recvData(eFd, &evt, data);
	// 			}
	// 			else if (evt.events & EPOLLOUT)
	// 				sendData(eFd, &evt, data);
	// 		}
	// 	}
	// }
	return 0;
}
