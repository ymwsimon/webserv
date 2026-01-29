/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   client.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mayeung <mayeung@student.42london.com>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/14 20:22:41 by mayeung           #+#    #+#             */
/*   Updated: 2026/01/29 17:28:49 by mayeung          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../include/client.hpp"
#include "../include/utils.hpp"

Client::Client(Service *ser) : service(ser)
{

}

Client::Client(const Client &right) : service(right.service)
{
	*this = right;
}

Client::~Client()
{
	
}

Client	&Client::operator=(const Client &right)
{
	if (this != &right)
	{
		incomingData = right.incomingData;
		requests = right.requests;
		responses = right.responses;
		service = right.service;
	}
	return *this;
}

int	Client::sendData(struct epoll_event *evt)
{
	Bytes	content;

	if (!requests.empty() && requests.front().complete())
	{
		if (responses.empty())
		{
			responses.push_back(Response(service, requests.front()));
			std::cout<<"new response"<<std::endl;
			requests.front().printRequest();
		}
		responses.front().processResponse();
		if (!responses.empty() && !responses.front().getResultPage().empty()
			&& (!responses.front().isCGI() || responses.front().isFinishWaitingStage()))
		{
			if (!requests.front().getBodyFilePath().empty()
				&& fileExist(requests.front().getBodyFilePath()))
				std::remove(requests.front().getBodyFilePath().c_str());
			if (!responses.front().getBodyFilePath().empty()
				&& fileExist(responses.front().getBodyFilePath()))
				std::remove(responses.front().getBodyFilePath().c_str());
			std::cout << responses.front().getStatusCode() << std::endl;;
			std::cout << responses.front().getResultPage().size() << std::endl;
			content = responses.front().getResultPage();
			std::cout << "sending out data" << std::endl;
			std::cout << "content" << std::endl;
			for (size_t i = 0; i < responses.front().getResultPage().size() && i < 200; ++i)
				std::cout << responses.front().getResultPage()[i];
			std::cout << std::endl;
			if (send(evt->data.fd, responses.front().getResultPage().data(),
				responses.front().getResultPage().size(), 0) < 0)
				std::cout << "error send data out" << std::endl;
			requests.pop_front();
			responses.pop_front();
			// if (statusCode == BAD_REQUEST)
				// return 0;
		}
	}
	return 1;
}

int Client::recvData(struct epoll_event *evt)
{
	Byte	buf[BUFFER_SIZE];
	int		readSize = 0;
	int		fd;

	readSize = recv(evt->data.fd, buf, BUFFER_SIZE, 0);
	// std::cout << "read size from socket: " << readSize << std::endl;
	if (readSize > 0)
	{
		(void)fd;
		// fd = open("indata", O_WRONLY | O_CREAT | O_APPEND, 0755);
		incomingData.insert(incomingData.end(), buf, buf + readSize);
		// write(fd, buf, readSize);
		// close(fd);
	}
	// if (!readSize || (readSize == 1 && (buf[0] == EOT || buf[0] == ((unsigned char)EOF))))
		// return 0;
	processData();
	return readSize;
}

void	Client::processResponseCgi(int op)
{
	if (responses.empty())
		return ;
	Response	&response = responses.front();

	response.processCgi(op);
}

Bytes::const_iterator	&Client::searchForNewLine(Bytes::const_iterator &it)
{
	it = searchPattern(getIncomingData(), CRLF);
	return it;
}

void	Client::processData()
{
	Bytes::const_iterator	it;

	while (searchForNewLine(it) != incomingData.end())
	{
		if (requests.empty() || requests.back().complete())
			requests.push_back(Request(incomingData.begin(), incomingData.end()));
		else
		{
			requests.back().setDataStart(incomingData.begin());
			requests.back().setDataEnd(incomingData.end());
		}
		requests.back().parseRequest();
		incomingData = Bytes(requests.back().getDataStart(), requests.back().getDataEnd());
	}
}

const Bytes	&Client::getIncomingData() const
{
	return incomingData;
}

const std::deque<Request>	&Client::getRequests() const
{
	return requests;
}

const std::deque<Response>	&Client::getResponses() const
{
	return responses;
}
