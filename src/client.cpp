/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   client.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mayeung <mayeung@student.42london.com>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/14 20:22:41 by mayeung           #+#    #+#             */
/*   Updated: 2025/11/26 12:30:55 by mayeung          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../include/client.hpp"
#include "../include/utils.hpp"

Client::Client(Service &ser) : service(ser)
{

}

Client::~Client()
{
	
}

int	Client::sendData(struct epoll_event *evt)
{
	Bytes	content;

	if (!requests.empty() && requests.front().complete())
	{
		requests.front().setMatchLocation(service.findMatchingRoute(requests.front()));
		requests.front().printRequest();
		responses.push_back(Response(service, requests.front()));
		
		if (responses.front().getErrorCode())
			content = defaultErrorPage();
		else if (!responses.front().getResultPage().empty())
			content = responses.front().getResultPage();
		else
			content = responses.front().getOKResponse();
		std::cout << "sending out data" << std::endl;
		std::cout << "content\n";
		for (size_t i = 0; i < content.size(); ++i)
			std::cout << content[i];
		std::cout << std::endl;
		if (send(evt->data.fd, content.data(), content.size(), 0) < 0)
			std::cout << "error send data out" << std::endl;
		requests.pop_front();
		responses.pop_front();
	}
	return 1;
}

int Client::recvData(struct epoll_event *evt)
{
	Byte	buf[BUFFER_SIZE];
	int		readSize = 0;

	readSize = recv(evt->data.fd, buf, BUFFER_SIZE, 0);
	if (readSize)
		std::cout << readSize << std::endl;
	if (readSize > 0)
	{
		incomingData.insert(incomingData.end(), buf, buf + readSize);
		// for (size_t i = 0; i < incomingData.size(); ++i)
		// 	std::cout << incomingData[i];
		// std::cout << std::endl;
	}
	processData();
	return readSize;
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
		try
		{
			if (requests.empty() || requests.back().complete())
				requests.push_back(Request(incomingData.begin(), incomingData.end()));
			else
			{
				requests.back().setDataStart(incomingData.begin());
				requests.back().setDataEnd(incomingData.end());
			}
			requests.back().parseRequest();
			incomingData = Bytes(reinterpret_cast<Bytes::iterator &>(requests.back().getDataStart()),
				incomingData.end());
		}
		catch(const std::exception& e)
		{
			std::cerr << e.what() << '\n';
		}
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

Client	&Client::operator=(const Client &right)
{
	//////////need to implement
	(void)right;
	return *this;
}
