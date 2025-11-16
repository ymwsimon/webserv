/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   request.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mayeung <mayeung@student.42london.com>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/14 23:12:55 by mayeung           #+#    #+#             */
/*   Updated: 2025/11/16 17:30:03 by mayeung          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../include/request.hpp"

Request::Request()
{

}

Request::~Request()
{
	
}

void	Request::parseMethod(const Bytes &newData)
{
	std::string				tmpMethod;
	Bytes::const_iterator	it;

	it = searchPattern(newData, SPACE);
	if (it != newData.cend())
	{
		tmpMethod = std::string(newData.begin(), it);
		if (tmpMethod == "POST" || tmpMethod == "GET" || tmpMethod == "DELETE")
		 	method = tmpMethod;
	}
}

void	Request::parseRoute(const Bytes &newData)
{
	(void)newData;
}

void	Request::parseHttpVersion(const Bytes &newData)
{
	(void)newData;
}

void	Request::parseRequestLine(const Bytes &newData)
{
	(void)newData;
}

void	Request::parseRequest(const Bytes &newData)
{
	while (!complete() && searchPattern(newData, CRLF) != newData.end())
	{
		
	}
}

bool	Request::complete()
{
	return requestStatus == COMPLETE;
}

void	Request::appendData(Bytes &newData, size_t size)
{
	(void)size;
	(void)newData;
	incomingData.insert(incomingData.end(), size);
}
