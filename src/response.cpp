/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   response.cpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mayeung <mayeung@student.42london.com>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/17 14:05:04 by mayeung           #+#    #+#             */
/*   Updated: 2025/11/22 02:07:58 by mayeung          ###   ########.fr       */
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
		pageStream = new std::ifstream(filePathStr.c_str(), std::ios_base::in);
		if (req.getFileName().empty() || (!pageStream->is_open() && !errorCode))
			errorCode = 404;
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

void	Response::printResponse() const
{
	std::cout << "Response: " << std::endl;
	if (!request.getMatchLocation())
		std::cout << "No match location" << std::endl;
	std::cout << "Error code: " << errorCode << std::endl;
}

Bytes	Response::getOKResponse()
{
	Bytes	res(BUFFER_SIZE);
	std::string			str;
	std::stringstream	strStream;

	pageStream->read((char *)res.data(), BUFFER_SIZE);
	pageStream->close();
	delete pageStream;
	str = "HTTP/1.1 200 OK\r\n"
			"Content-Type: text/html\r\n"
			"Content-Length: ";
	strStream << res.size();
	str += strStream.str();
	str += "\r\n\r\n";
	str += std::string(res.begin(), res.end());
	return Bytes(str.begin(), str.end());
}
