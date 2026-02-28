/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   client.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mayeung <mayeung@student.42london.com>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/14 20:22:41 by mayeung           #+#    #+#             */
/*   Updated: 2026/02/25 18:54:22 by mayeung          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../include/client.hpp"
#include "../include/utils.hpp"

Client::Client(Service *ser, int fd) : service(ser)
{
	socketFd = fd;
	clientError = false;
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
		socketFd = right.socketFd;
		clientError = right.clientError;
	}
	return *this;
}

int	Client::sendData(struct epoll_event *evt)
{
	if (isOkToSendData())
	{
		Request		&request = requests.front();
		Response	&response = responses.front();

		if (response.isFinishWaitingStage() || !response.isCGI())
		{
			if (!request.getBodyFilePath().empty()
				&& fileExist(request.getBodyFilePath()))
				std::remove(request.getBodyFilePath().c_str());
			if (!response.getBodyFilePath().empty()
				&& fileExist(response.getBodyFilePath()))
				std::remove(response.getBodyFilePath().c_str());
		}
		size_t	size = std::min((size_t)TRANSFER_SIZE, response.getResultPage().size());
		ssize_t	sentSize;

		sentSize = send(evt->data.fd, response.getResultPage().data(), size, 0);
		if (sentSize < 0)
		{
			std::cout << "error send data out" << std::endl;
			clientError = true;
		}
		else
		{
			// for (size_t i = 0; i < response.getResultPage().size() && i < 500; ++i)
			// 	std::cout<<response.getResultPage()[i];
			// std::cout<<std::endl;
			// writeOutData(sentSize);
			response.setResultSent(true);
			response.removeNCharFromResultPage(sentSize);
		}
	}
	return 1;
}

int Client::recvData(struct epoll_event *evt)
{
	int		readSize = 0;

	readSize = recv(evt->data.fd, buf, BUFFER_SIZE, 0);

	// std::cout << "read size from socket: " << readSize << std::endl;
	if (readSize > 0)
	{
		incomingData.insert(incomingData.end(), buf, buf + readSize);
		// writeIncomingData(readSize);
		if (!responses.empty() && responses.front().isCGI() && responses.front().isWaitingStage())
			responses.front().updateCgiActiveTime();
		
	}
	if (readSize < 0)
	{
		clientError = true;
	}
	if (!readSize || (readSize == 1 && (buf[0] == EOT || buf[0] == ((unsigned char)EOF))))
		return 0;
	return readSize;
}

ssize_t	Client::processResponseCgi(int op)
{
	ssize_t		res = 0;

	if (!responses.empty())
	{
		Response	&response = responses.front();

		if (op == EXTRACT_PIPE)
			res = response.extractResultFromCgiPipe();
		else if (op == WRITE_PIPE)
			res = response.writeDataToCgiPipe();
		else
			res = response.processCgi(op);
		if (res < 0)
			clientError = true;
	}
	return res;
}

Bytes::const_iterator	&Client::searchForNewLine(Bytes::const_iterator &it)
{
	it = searchPattern(getIncomingData(), CRLF);
	return it;
}

void	Client::processRequest()
{
	Bytes::const_iterator	it;

	if (!requests.empty() && requests.front().complete())
		return ;
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

void	Client::processResponse()
{
	if (!requests.empty() && (requests.front().complete() || requests.front().isWaitingChunk()))
	{
		Request		&request = requests.front();
		Response	&response = responses.front();

		if (!responses.empty() && response.isFinishWaitingStage() && response.getCgiRes().empty())
			return ;
		if (responses.empty())
		{
			responses.push_back(Response(service, request));
			response = responses.front();
			response.routeMatchingCheckLocationLimitationDetermineType();
			// request.printRequest();
		}
		if (response.isChunkMode())
		{
			response.checkBodySize();
		}
		response.updateResultPage();
	}
}

void	Client::writeIncomingData(int readSize)
{
	int	fd;

	fd = open("indata", O_WRONLY | O_CREAT | O_APPEND, 0755);
	if (write(fd, buf, readSize))
	{}
	close(fd);
}

void	Client::writeOutData(size_t size)
{
	int	fd;

	fd = open("outdata", O_CREAT | O_APPEND | O_WRONLY, 0777);
	if (fd >= 0)
	{
		if (write(fd, responses.front().getResultPage().data(), size))
		{}
		close(fd);
	}
	else
		std::cout<<"open outdata fail"<<std::endl;
}

bool	Client::timeToCloseCgiInPipe() const
{
	return !requests.empty()
		&& ((requests.front().complete()
		&& requests.front().getBodyLength() == responses.front().getByteWritten()
		&& !responses.empty()) || responses.front().isFinishWaitingStage());
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
		&& ((responses.front().isFinishWaitingStage()
		&& responses.front().isCgiOutPipeDrained()) || clientError);
		// && responses.front().getCgiRes().empty()
		// && responses.front().getResultPage().empty();
}

bool	Client::isOkToSendData() const
{
	if (requests.empty() || responses.empty())
		return false;

	const Request	&request = requests.front();
	const Response	&response = responses.front();

	return (!response.getResultPage().empty()
			&& ((request.complete() && !response.isCGI())
				|| (((request.isWaitingChunk() || request.complete())
					&& response.isCGI()
					&& (response.isWaitingStage() || response.isFinishWaitingStage())))));
}

bool	Client::isOkToRemoveRequestResponse() const
{
	if (requests.empty() || responses.empty())
		return false;

	const Request	&request = requests.front();
	const Response	&response = responses.front();

	return (request.complete() && response.isResultSent()
			&& ((!response.isCGI() && (!response.isFileType() || response.isFileAllRead()))
				|| (response.isFinishWaitingStage()
					&& response.getResultPage().empty()
					&& response.getCgiRes().empty()
					&& response.isCgiOutPipeDrained()
					&& (response.isEndChunkAppended() || !response.isChunkMode()))));
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

int	Client::getSocketFd() const
{
	return socketFd;
}

bool	Client::getClientError() const
{
	return clientError;
}

void	Client::removeReqResPair()
{
	std::cout<<"removing req response"<<std::endl;
	requests.pop_front();
	responses.pop_front();
}

void	Client::processRequestResponse()
{
	// std::cout<<"client with fd processing start:"<<socketFd<<std::endl;
	processRequest();
	processResponse();
	// std::cout<<"client with fd processing fin:"<<socketFd<<std::endl;
}

void	Client::setCgiStageToWaiting()
{
	if (responses.front().isAddFdStage())
		responses.front().setCgiStage(WAITING_HEADER);
}

void	Client::setClientError(bool err)
{
	clientError = err;
}
