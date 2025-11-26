/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   response.cpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mayeung <mayeung@student.42london.com>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/17 14:05:04 by mayeung           #+#    #+#             */
/*   Updated: 2025/11/26 12:27:14 by mayeung          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../include/response.hpp"

Response::Response(const Response &other) : service(other.service), request(other.request)
{
	errorCode = other.getErrorCode();
	pageStream =  other.pageStream;
}

Response::Response(Service &ser, Request &req) : service(ser), request(req)
{
	std::string	filePathStr;

	errorCode = req.getErrorCode();

	if (!errorCode && req.getMatchLocation())
	{
		filePathStr = mergeFullPath(req.getMatchLocation()->getRootFolder(),
			req.getPaths(), req.getFileName());
		std::cout << "File path str: " << filePathStr << std::endl;
		if (!req.getFileName().empty() && !isDir(filePathStr))
		{
			try 
			{
				std::cout << "opening " << filePathStr << " as file\n";
				pageStream = new std::ifstream(filePathStr.c_str(), std::ios_base::in);
				if (!pageStream->good())
				{
					std::cout << "can't open " << filePathStr << " as file\n";
					errorCode = 404;
					pageStream->close();
					pageStream = NULL;
				}
			}
			catch (std::exception &e)
			{
				if (!errorCode)
					errorCode = 404;
			}
		}
		else
		{
			std::cout << "dir\n" << filePathStr << std::endl;
			pageStream = req.getMatchLocation()->tryOpenIndexPages(filePathStr);
			if (!pageStream)
			{
				std::cout << "index page not found\n";
				std::cout << "list folder content.." << filePathStr << std::endl;
				if (req.getMatchLocation()->getAutoIndex())
					resultPage = req.getMatchLocation()->generateIndexPages(filePathStr, mergeFullPath("", req.getPaths(), req.getFileName()));
				else if (!errorCode)
					errorCode = 404;
			}
		}
	}
}

Response::~Response()
{

}

Response	&Response::operator=(const Response &right)
{
	////////////////////////////////////////need to implement
	service = right.service;
	request = right.request;
	pageStream = right.pageStream;
	errorCode = right.errorCode;
	return *this;
}

const int	&Response::getErrorCode() const
{
	return errorCode;
}

const std::ifstream	*Response::getPageStream()
{
	return pageStream;
}

const Bytes	&Response::getResultPage() const
{
	return resultPage;
}

void	Response::printResponse() const
{
	std::cout << "Response: " << std::endl;
	if (!request.getMatchLocation())
		std::cout << "No match location" << std::endl;
	std::cout << "Error code: " << errorCode << std::endl;
}

Bytes	Response::getOKResponse()
{
	Bytes	buf(BUFFER_SIZE);
	Bytes	res;
	std::string			str;
	std::stringstream	strStream;

	pageStream->read((char *)buf.data(), BUFFER_SIZE);
	pageStream->close();
	str = "HTTP/1.1 200 OK\r\n"
			"Content-Type: text/html\r\n"
			"Content-Length: ";
	strStream << pageStream->gcount();
	str += strStream.str();
	str += "\r\n\r\n";
	res.insert(res.end(), str.begin(), str.end());
	res.insert(res.end(), buf.begin(), buf.begin() + pageStream->gcount());
	delete pageStream;
	pageStream = NULL;
	return res;
}
