/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   request.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mayeung <mayeung@student.42london.com>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/14 23:12:55 by mayeung           #+#    #+#             */
/*   Updated: 2025/11/16 23:24:04 by mayeung          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../include/request.hpp"

Request::Request(Bytes::const_iterator start, Bytes::const_iterator end) : newDataStart(start), newDataEnd(end)
{
	requestStatus = METHOD;
}

Request::~Request()
{
	
}

std::string	Request::parseReqLineSegment(const Bytes &delimiter)
{
	Bytes::const_iterator	it;
	std::string				res;
	int						err = 0;

	it = searchPattern(newDataStart, newDataEnd, delimiter);
	if (it != newDataEnd)
	{
		res = std::string(newDataStart, it);
		if (requestStatus == METHOD)
		{
			if (std::find(validMethod.begin(), validMethod.end(), res) == validMethod.end())
				err = 400;
		}
		else if (requestStatus == ROUTE)
		{
			//check start with /? and empty route
		}
		else if (requestStatus == HTTPVERSION)
		{
			if (std::find(validHttpVersion.begin(), validHttpVersion.end(), res) == validHttpVersion.end())
				err = 400;
		}
		newDataStart = it + delimiter.size();
		++requestStatus;
		if (!errorCode)
			errorCode = err;
	}
	return res;
}

void	Request::parseRequestLine()
{
	method = parseReqLineSegment(SPACE);
	route = parseReqLineSegment(SPACE);
	httpVer = parseReqLineSegment(CRLF);
}

void	Request::parseRequestHeader()
{
	std::string				key;
	std::string				value;
	Bytes::const_iterator	colonIt;
	Bytes::const_iterator	crlfIt;

	colonIt = searchPattern(newDataStart, newDataEnd, COLON);
	if (colonIt != newDataEnd)
	{
		key = std::string(newDataStart, colonIt);
		crlfIt = searchPattern(colonIt + 1, newDataEnd, CRLF);
		if (colonIt != newDataEnd)
		{
			value = std::string(colonIt + 1, crlfIt);
			headers.insert(std::make_pair(key, value));
			newDataStart = crlfIt;
		}
		else if (!errorCode)
			errorCode = 400;
	}
	else if (!errorCode)
		errorCode = 400;
}

void	Request::parseBody()
{
	body.insert(body.end(), newDataStart, newDataEnd);
	++requestStatus;
}

void	Request::parseRequest()
{
	Bytes::const_iterator	it;

	while (!complete() && (it = searchPattern(newDataStart, newDataEnd, CRLF)) != newDataEnd)
	{
		if (it == newDataStart)
			++requestStatus;
		if (requestStatus == METHOD)
			{std::cout << "parse request line" << std::endl; parseRequestLine();}
		else if (requestStatus == HEADERS)
			{std::cout << "parse header" << std::endl;; parseRequestHeader();}
		else if (requestStatus == BODY)
			parseBody();
		newDataStart = it + 2;
	}
}

bool	Request::complete()
{
	return requestStatus == COMPLETE;
}

void	Request::printRequest()
{
	std::cout << "Request:" << std::endl;
	std::cout << "\tMethod:" << method << std::endl;
	std::cout << "\tRoute:" << route << std::endl;
	std::cout << "\tVersion:" << httpVer << std::endl;
	std::cout << "\tHeader:" << std::endl;
	for (std::map<std::string, std::string>::iterator it = headers.begin(); it != headers.end(); ++it)
		std::cout << "\t\t" << it->first << " : " << it->second << std::endl;
	std::cout << std::endl;
	for (size_t i = 0; i < body.size(); ++i)
		std::cout << body[i];
	std::cout << std::endl;

}

void	Request::setDataStart(Bytes::const_iterator &s)
{
	newDataStart = s;
}

void	Request::setDataEnd(Bytes::const_iterator &e)
{
	newDataEnd = e;
}

Bytes::const_iterator	&Request::getDataStart()
{
	return newDataStart;
}

Bytes::const_iterator	&Request::getDataEnd()
{
	return newDataEnd;
}
