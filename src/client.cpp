/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   client.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mayeung <mayeung@student.42london.com>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/14 20:22:41 by mayeung           #+#    #+#             */
/*   Updated: 2025/11/16 23:29:55 by mayeung          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */


#include "../include/client.hpp"
#include "../include/utils.hpp"

Client::Client()
{

}

Client::~Client()
{
	
}

int	Client::sendData(struct epoll_event *evt)
{
	std::string			str;
	std::string			content;
	std::stringstream	strStream;

	if (!requests.empty())
	{
		requests.pop_front();
		responses.push_back(Response());
	}
	if (!responses.empty())
	{
		responses.pop_front();
		content = "<!DOCTYPE html>"
					"<html>"
					"<head>"
					"<meta content=\"text/html; charset=ISO-8859-1\" http-equiv=\"content-type\">"
					"<title>WebServer</title></head>"
					"<body>"
					"Testing"
					"</body></html>";
		str = "HTTP/1.1 200 OK\r\n"
				"Content-Type: text/html\r\n"
				"Content-Length: ";
		strStream << content.length();
		str += strStream.str();
		str += "\r\n\r\n";
		str += content;
		std::cout << "sending out data" << std::endl;
		if (send(evt->data.fd, str.c_str(), str.length(), 0) < 0)
			std::cout << "error send data out" << std::endl;
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
			requests.back().parseRequest();
			incomingData = Bytes(requests.back().getDataStart(), incomingData.cend());
			if (requests.back().complete())
				requests.back().printRequest();
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
