/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   client.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mayeung <mayeung@student.42london.com>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/14 20:22:41 by mayeung           #+#    #+#             */
/*   Updated: 2025/11/17 14:13:47 by mayeung          ###   ########.fr       */
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

std::string	staticPage()
{
	std::string			str;
	std::string			content;
	std::stringstream	strStream;

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
	return str;
}

int	Client::sendData(struct epoll_event *evt)
{
	std::string			content;

	if (!requests.empty() && requests.front().complete())
	{
		requests.front().printRequest();
		requests.pop_front();
		responses.push_back(Response());
	}
	if (!responses.empty())
	{
		content = staticPage();
		std::cout << "sending out data" << std::endl;
		if (send(evt->data.fd, content.c_str(), content.length(), 0) < 0)
			std::cout << "error send data out" << std::endl;
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
			// if (requests.back().complete())
			// 	requests.back().printRequest();
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
