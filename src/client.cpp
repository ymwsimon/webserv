/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   client.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mayeung <mayeung@student.42london.com>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/14 20:22:41 by mayeung           #+#    #+#             */
/*   Updated: 2025/11/16 17:29:30 by mayeung          ###   ########.fr       */
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
	Bytes::iterator 	it;

	///should look for complete request
	it = std::search(incomingData.begin(), incomingData.end(), DOUBLE_CRLF.begin(), DOUBLE_CRLF.end());
	if (it != incomingData.end())
	{
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
		std::cout << "request" << std::endl;
		for (size_t i = 0; i < incomingData.size(); ++i)
			std::cout << incomingData[i];
		std::cout << std::endl;
		std::cout << "sending out data" << std::endl;
		incomingData = std::vector<unsigned char>(it + 4, incomingData.end());
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
		for (size_t i = 0; i < incomingData.size(); ++i)
			std::cout << incomingData[i];
		std::cout << std::endl;
	}
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
	Request					*req;

	while (searchForNewLine(it) != incomingData.end())
	{
		if (requests.empty() || requests.back().complete())
			requests.push_back(Request());
		req = &requests.back();
		req->appendData(incomingData, std::distance(incomingData.cbegin(), it));
		incomingData = Bytes(it + 2, incomingData.cend());
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
