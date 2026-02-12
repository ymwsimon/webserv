/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   client.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mayeung <mayeung@student.42london.com>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/14 20:22:41 by mayeung           #+#    #+#             */
/*   Updated: 2026/02/12 18:02:42 by mayeung          ###   ########.fr       */
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
	Bytes	content;

	if (!requests.empty() && (requests.back().complete() || requests.back().isWaitingChunk()))
	{
		// Request	&request = requests.back();

		if (responses.empty())
		{
			responses.push_back(Response(service, requests.front()));
			// std::cout<<"new response"<<std::endl;
			requests.front().printRequest();
		}
		Response	&response = responses.front();

		// if (response.isCGI())
		// 	response.processCgi(PROCESS_DATA);
		// else
		response.processResponse();
		// if (!responses.empty() && !responses.front().getResultPage().empty())
			// && (!responses.front().isCGI() || responses.front().isChunkMode()))
		if (!response.getResultPage().empty()
			&& ((requests.front().complete() && !response.isCGI())
				|| (((requests.front().isWaitingChunk() || requests.front().complete())
					&& response.isCGI() && response.statusOK()
					&& (response.isWaitingStage() || response.isFinishWaitingStage())))))
		{
			// std::cout<<"req size:"<<requests.size()<<" req f waiting chunk:" << requests.front().isWaitingChunk()<<  std::endl;
			// std::cout<<"req status:"<<requests.front().getStatusCode()<<" req comp:"<<requests.front().complete() <<std::endl;
			// std::cout<<"res is cgi:"<<responses.front().isCGI() <<" res got enough data:" << responses.front().gotEnoughChunkDataToSent()<< " res is finish waiting:"<<responses.front().isFinishWaitingStage()<<  std::endl;
			if (responses.front().isFinishWaitingStage() || !responses.front().isCGI())
			{
				if (!requests.front().getBodyFilePath().empty()
					&& fileExist(requests.front().getBodyFilePath()))
					std::remove(requests.front().getBodyFilePath().c_str());
				if (!responses.front().getBodyFilePath().empty()
					&& fileExist(responses.front().getBodyFilePath()))
					std::remove(responses.front().getBodyFilePath().c_str());
			}
			std::cout << responses.front().getStatusCode() << std::endl;;
			std::cout << responses.front().getResultPage().size() << std::endl;
			std::cout << "sending out data" << std::endl;
			std::cout << "content" << std::endl;
			for (size_t i = 0; i < responses.front().getResultPage().size() && i < 200; ++i)
				std::cout << responses.front().getResultPage()[i];
			std::cout << std::endl;
			size_t	size = std::min((size_t)TRANSFER_SIZE, responses.front().getResultPage().size());
			// if (responses.front().isFinishWaitingStage())
			// 	size = responses.front().getResultPage().size();
			int	fd;
			(void)fd;
			// fd = open("outdata", O_CREAT | O_APPEND | O_WRONLY, 0777);
			// if (fd >= 0)
			// {
			// 	write(fd, responses.front().getResultPage().data(), size);
			// 	close(fd);
			// }
			// else
			// 	std::cout<<"open outdata fail"<<std::endl;
			if (send(evt->data.fd, responses.front().getResultPage().data(),
				size, 0) < 0)
				std::cout << "error send data out" << std::endl;
			if (responses.front().statusOK() && responses.front().isCGI()
				&& responses.front().isChunkMode())
			{
				responses.front().removeNCharFromResultPage(size);
			}
			if (requests.front().complete()
				&& (!responses.front().isChunkMode()
					|| !responses.front().isCGI()
					|| (responses.front().isFinishWaitingStage() && responses.front().getResultPage().size() == 0)))
			{
				std::cout<<"removing req response"<<std::endl;
				requests.pop_front();
				// responses.front().clearResultPage();
				responses.pop_front();
			}
			// if (statusCode == BAD_REQUEST)
				// return 0;
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
		responses.front().updateCgiActiveTime();
		// if (readSize < BUFFER_SIZE)
		// 	break ;
		// write(fd, buf, readSize);
		// close(fd);
		processData();
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
		response.extractResultFromCgiPipe();
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
		if (requests.empty() || requests.back().complete())
			requests.push_back(Request(incomingData.begin(), incomingData.end(), incomingData));
		else
		{
			requests.back().setDataStart(incomingData.begin());
			requests.back().setDataEnd(incomingData.end());
			requests.back().setIncomingData(incomingData);
		}
		requests.back().parseRequest();
		size_t	size = std::distance(static_cast<Bytes::const_iterator>(incomingData.begin()),
			requests.back().getDataStart());
		incomingData.erase(incomingData.begin(), incomingData.begin() + size);
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
