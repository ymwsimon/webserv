/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   request.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mayeung <mayeung@student.42london.com>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/14 23:12:55 by mayeung           #+#    #+#             */
/*   Updated: 2025/12/10 18:04:17 by mayeung          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../include/request.hpp"

std::string Request::valMet[3] = {"POST", "GET", "DELETE"};
std::vector<std::string>	Request::validMethod = std::vector<std::string>(Request::valMet, Request::valMet + 3);
std::string Request::valVer[1] = {"HTTP/1.1"};
std::vector<std::string>	Request::validHttpVersion = std::vector<std::string>(Request::valVer, Request::valVer + 1);

Request::Request(Bytes::const_iterator start, Bytes::const_iterator end) : newDataStart(start), newDataEnd(end)
{
	requestStatus = METHOD;
	errorCode = 0;
	bodyLength = 0;
	matchLocation = NULL;
}

Request::Request(const Request &right)
{
	*this = right;
}

Request::~Request()
{
	
}

Request	&Request::operator=(const Request &right)
{
	if (this != &right)
	{
		method = right.method;
		route = right.route;
		paths = right.paths;
		fileName = right.fileName;
		httpVer = right.httpVer;
		headers = right.headers;
		body = right.body;
		newDataStart = right.newDataStart;
		newDataEnd = right.newDataEnd;
		errorCode = right.errorCode;
		requestStatus = right.requestStatus;
		bodyLength = right.bodyLength;
		matchLocation = right.matchLocation;
	}
	return *this;
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

void	Request::splitRoute()
{
	std::pair<std::vector<std::string>, std::string>	splitRes;
	splitRes = splitPath(route);
	paths = splitRes.first;
	fileName = splitRes.second;
}

void	Request::parseRequestLine()
{
	method = parseReqLineSegment(SPACE);
	route = parseReqLineSegment(SPACE);
	splitRoute();
	httpVer = parseReqLineSegment(CRLF);
}

void	Request::extractContentLength(std::string &len)
{
	std::istringstream	ss(len);

	try
	{
		ss >> bodyLength;
	}
	catch (std::exception &e)
	{
		(void)e;
		if (!errorCode)
			errorCode = 400;
	}
}

void	Request::parseRequestHeader()
{
	std::string				key;
	std::string				value;
	Bytes::const_iterator	colonIt;
	Bytes::const_iterator	crlfIt;

	colonIt = searchPattern(newDataStart, newDataEnd, COLON);
	if (colonIt != newDataEnd && (colonIt != newDataStart))
	{
		key = std::string(newDataStart, colonIt);
		key = trim(key);
		crlfIt = searchPattern(colonIt + COLON.size(), newDataEnd, CRLF);
		if (colonIt != newDataEnd)
		{
			value = std::string(colonIt + COLON.size(), crlfIt);
			value = trim(value);
			if (key == "Content-Length")
				extractContentLength(value);
			headers.insert(std::make_pair(key, value));
			newDataStart = crlfIt + CRLF.size();
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
	if (body.size() >= bodyLength)
		++requestStatus;
}

void	Request::parseRequest()
{
	Bytes::const_iterator	it;

	while (!complete() && (it = searchPattern(newDataStart, newDataEnd, CRLF)) != newDataEnd)
	{
		if (it == newDataStart && (newDataStart != newDataEnd))
			++requestStatus;
		if (requestStatus == METHOD)
			{std::cout << "parse request line" << std::endl; parseRequestLine();}
		else if (requestStatus == HEADERS)
			{std::cout << "parse header" << std::endl;; parseRequestHeader();}
		else if (requestStatus == BODY)
			parseBody();
		newDataStart = it + CRLF.size();
	}
}

bool	Request::complete() const
{
	return requestStatus == COMPLETE;
}

void	Request::printRequest() const
{
	std::cout << "Request:" << std::endl;
	std::cout << "\tMethod:" << method << std::endl;
	std::cout << "\tRoute:" << route << std::endl;
	std::cout << "\tSplit route:";
	for (std::vector<std::string>::const_iterator it = paths.begin(); it != paths.end(); ++it)
		std::cout << " ,"[it != paths.begin()] << *it;
	std::cout << std::endl;
	std::cout << "\tFile name: " << fileName << std::endl;
	std::cout << "\tVersion:" << httpVer << std::endl;
	std::cout << "\tHeader:" << std::endl;
	for (std::map<std::string, std::string>::const_iterator it = headers.begin(); it != headers.end(); ++it)
		std::cout << "\t\t" << it->first << " : " << it->second << std::endl;
	std::cout << std::endl;
	for (size_t i = 0; i < body.size(); ++i)
		std::cout << body[i];
	std::cout << std::endl;
	if (matchLocation)
	{
		std::cout << "\tMatched Location:" << std::endl;
		matchLocation->printLocation();
	}
}

void	Request::setDataStart(Bytes::const_iterator s)
{
	newDataStart = s;
}

void	Request::setDataEnd(Bytes::const_iterator e)
{
	newDataEnd = e;
}

void	Request::setMatchLocation(const Location *loc)
{
	matchLocation = loc;
}

const std::string	&Request::getMethod() const
{
	return method;
}

const std::string	&Request::getRoute() const
{
	return route;
}

const std::string	&Request::getHttpVer() const
{
	return httpVer;
}

const std::map<std::string, std::string>	&Request::getHeaders() const
{
	return headers;
}

const Bytes	&Request::getBody() const
{
	return body;
}

const int	&Request::getErrorCode() const
{
	return errorCode;
}

const size_t	&Request::getBodyLength() const
{
	return bodyLength;
}

const std::vector<std::string>	&Request::getPaths() const
{
	return paths;
}

const std::string	&Request::getFileName() const
{
	return fileName;
}

Bytes::const_iterator	Request::getDataStart() const
{
	return newDataStart;
}

Bytes::const_iterator	Request::getDataEnd() const
{
	return newDataEnd;
}

const Location	*Request::getMatchLocation() const
{
	return matchLocation;
}
