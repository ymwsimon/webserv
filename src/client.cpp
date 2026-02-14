/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   client.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mayeung <mayeung@student.42london.com>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/14 20:22:41 by mayeung           #+#    #+#             */
/*   Updated: 2026/02/13 23:47:03 by mayeung          ###   ########.fr       */
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
	incomingData.clear();	
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
	if (!requests.empty() && (requests.front().complete() || requests.front().isWaitingChunk()))
	{
		Request		&request = requests.front();
		Response	&response = responses.front();

		if (responses.empty())
		{
			responses.push_back(Response(service, request));
			response = responses.front();
			response.routeMatchingCheckLocationLimitationDetermineType();
			request.printRequest();
		}
		if (response.isCGI())
			response.checkBodySize();
		response.updateResultPage();
		if (!response.getResultPage().empty()
			&& ((request.complete() && !response.isCGI())
				|| (((request.isWaitingChunk() || request.complete())
					&& response.isCGI() && response.statusOK()
					&& (response.isWaitingStage() || response.isFinishWaitingStage())))))
		{
			if (response.isFinishWaitingStage() || !response.isCGI())
			{
				if (!request.getBodyFilePath().empty()
					&& fileExist(request.getBodyFilePath()))
					std::remove(request.getBodyFilePath().c_str());
				if (!response.getBodyFilePath().empty()
					&& fileExist(response.getBodyFilePath()))
					std::remove(response.getBodyFilePath().c_str());
			}
			std::cout << response.getStatusCode() << std::endl;;
			std::cout << response.getResultPage().size() << std::endl;
			std::cout << "sending out data" << std::endl;
			std::cout << "content" << std::endl;
			for (size_t i = 0; i < response.getResultPage().size() && i < 200; ++i)
				std::cout << response.getResultPage()[i];
			std::cout << std::endl;
			size_t	size = std::min((size_t)TRANSFER_SIZE, response.getResultPage().size());
			if (false)
			{
				int	fd;

				fd = open("outdata", O_CREAT | O_APPEND | O_WRONLY, 0777);
				if (fd >= 0)
				{
					write(fd, responses.front().getResultPage().data(), size);
					close(fd);
				}
				else
					std::cout<<"open outdata fail"<<std::endl;
			}
			if (send(evt->data.fd, response.getResultPage().data(),
				size, 0) < 0)
				std::cout << "error send data out" << std::endl;
			response.setResultSent(true);
			response.removeNCharFromResultPage(size);
			if (request.complete()
				&& (!response.isChunkMode()
					|| !response.isCGI()
					|| (response.isFinishWaitingStage()
						&& response.getResultPage().size() == 0
						&& response.getCgiRes().empty()
						&& response.isCgiOutPipeDrained())))
			{
				std::cout<<"removing req response"<<std::endl;
				requests.pop_front();
				responses.pop_front();
			}
		}
	}
	return 1;
}

int Client::recvData(struct epoll_event *evt)
{
	// Byte	buf[BUFFER_SIZE];
	int		readSize = 0;
	int		fd;

	// while ((readSize = recv(evt->data.fd, buf, BUFFER_SIZE, MSG_DONTWAIT)) > 0)
	readSize = recv(evt->data.fd, buf, BUFFER_SIZE, 0);

	// std::cout << "read size from socket: " << readSize << std::endl;
	if (readSize > 0)
	{
		(void)fd;
		// fd = open("indata", O_WRONLY | O_CREAT | O_APPEND, 0755);
		incomingData.insert(incomingData.end(), buf, buf + readSize);
		processData();
		if (!responses.empty() && responses.front().isCGI() && responses.front().isWaitingStage())
			responses.front().updateCgiActiveTime();
		// if (readSize < BUFFER_SIZE)
		// 	break ;
		// write(fd, buf, readSize);
		// close(fd);
		
	}
	// if (!readSize || (readSize == 1 && (buf[0] == EOT || buf[0] == ((unsigned char)EOF))))
		// return 0;
	return readSize;
}

void	Client::processResponseCgi(int op)
{
	if (responses.empty())
		return ;

	Response	&response = responses.front();

	if (op == EXTRACT_PIPE)
	{
		// {
		// 	while (response.extractResultFromCgiPipe() > 0)
		// 		;
		// }
		// else
			response.extractResultFromCgiPipe();

	}
	else if (op == WRITE_PIPE)
		response.writeDataToCgiPipe();
	else
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
		if (requests.empty())
			requests.push_back(Request(incomingData.begin(), incomingData.end(), incomingData));
		requests.front().setDataStart(incomingData.begin());
		requests.front().setDataEnd(incomingData.end());
		requests.front().setIncomingData(incomingData);
		requests.front().parseRequest();
		size_t	size = std::distance(static_cast<Bytes::const_iterator>(incomingData.begin()),
			requests.front().getDataStart());
		incomingData.erase(incomingData.begin(), incomingData.begin() + size);
	}
}

bool	Client::timeToCloseCgiInPipe() const
{
	return !requests.empty()
		&& requests.front().complete()
		&& requests.front().getBody().size() == 0;
}

bool	Client::timeToAddCgiPipeToEpoll() const
{
	return !responses.empty()
		&& responses.front().statusOK()
		&& responses.front().isAddFdStage();
}

bool	Client::timeToRemoveCgiPipeFromEpoll() const
{
	return !responses.empty()
		&& responses.front().isCgiOutPipeDrained()
		&& responses.front().getCgiRes().empty()
		&& responses.front().getResultPage().empty();
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

int	Client::getResponseCgiInPipeFd() const
{
	if (responses.empty() || !responses.front().isCGI())
		return -1;
	return responses.front().getCgiInFd();
}

int	Client::getResponseCgiOutPipeFd() const
{
	if (responses.empty() || !responses.front().isCGI())
		return -1;
	return responses.front().getCgiOutFd();
}

void	Client::removeReqResPair()
{
	requests.pop_front();
	responses.pop_front();
}
