/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   request.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mayeung <mayeung@student.42london.com>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/14 23:12:55 by mayeung           #+#    #+#             */
/*   Updated: 2026/02/01 15:21:04 by mayeung          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../include/request.hpp"

std::string Request::valMet[4] = {"POST", "GET", "DELETE", "HEAD"};
std::vector<std::string>	Request::validMethod = std::vector<std::string>(Request::valMet, Request::valMet + 4);
std::string Request::valVer[1] = {"HTTP/1.1"};
std::vector<std::string>	Request::validHttpVersion = std::vector<std::string>(Request::valVer, Request::valVer + 1);

Request::Request(Bytes::const_iterator start, Bytes::const_iterator end) : newDataStart(start), newDataEnd(end)
{
	requestStatus = METHOD;
	statusCode = HTTP_OK;
	bodyLength = 0;
	expectedChunkSize = -1;
	bodyFd = -1;
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
		httpVer = right.httpVer;
		headers = right.headers;
		body = right.body;
		newDataStart = right.newDataStart;
		newDataEnd = right.newDataEnd;
		statusCode = right.statusCode;
		requestStatus = right.requestStatus;
		bodyLength = right.bodyLength;
		bodyFilePath = right.bodyFilePath;
		expectedChunkSize = right.expectedChunkSize;
		bodyFd = right.bodyFd;
	}
	return *this;
}

std::string	Request::parseReqLineSegment(const Bytes &delimiter)
{
	Bytes::const_iterator	it;
	std::string				res;

	if (complete())
		return res;
	it = searchPattern(newDataStart, newDataEnd, delimiter);
	if (it != newDataEnd)
	{
		res = std::string(newDataStart, it);
		if (requestStatus == METHOD)
		{
			if (std::find(validMethod.begin(), validMethod.end(), res) == validMethod.end())
			{
				setStatusCode(BAD_REQUEST);
				requestStatus = COMPLETE;
			}
		}
		else if (requestStatus == ROUTE)
		{
			if (res.empty() || res[0] != '/')
			{
				setStatusCode(BAD_REQUEST);
				requestStatus = COMPLETE;
			}
		}
		else if (requestStatus == HTTPVERSION)
		{
			// std::cout << "http version: " << res << std::endl;
			// if (std::find(validHttpVersion.begin(), validHttpVersion.end(), res) == validHttpVersion.end())
			// if (res.empty())
			// {
			// 	setStatusCode(BAD_REQUEST);
			// 	requestStatus = COMPLETE;
			// }
		}
		newDataStart = it + delimiter.size();
		if (requestStatus != COMPLETE)
			++requestStatus;
	}
	return res;
}

void	Request::splitRoute()
{
	paths = splitPath(route);
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
		if (statusOK())
		{
			bodyLength = 0;
			setStatusCode(BAD_REQUEST);
			requestStatus = COMPLETE;
		}
	}
}

bool	Request::isPostMethod() const
{
	return method == "POST";
}

bool	Request::isPutMethod() const
{
	return method == "PUT";
}

bool	Request::switchToChunkMode()
{
	return headers.count(TRANSFER_ENDCODING) > 0
		&& headers.at(TRANSFER_ENDCODING) == CHUNKED;
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
		key = stringToLowerCase(key);
		if (key.empty() || headers.count(key) > 0)
		{
			requestStatus = COMPLETE;
			setStatusCode(BAD_REQUEST);
			return ;
		}
		crlfIt = searchPattern(colonIt + COLON.size(), newDataEnd, CRLF);
		if (colonIt != newDataEnd)
		{
			value = std::string(colonIt + COLON.size(), crlfIt);
			value = trim(value);
			if (value.empty())
			{
				requestStatus = COMPLETE;
				setStatusCode(BAD_REQUEST);
				return ;
			}
			if (key == CONTENT_LENGTH && (isPostMethod() || isPutMethod()))
				extractContentLength(value);
			headers.insert(std::make_pair(key, value));
			newDataStart = crlfIt + CRLF.size();
		}
		else
		{
			requestStatus = COMPLETE;
			setStatusCode(BAD_REQUEST);
		}
	}
	else
	{
		requestStatus = COMPLETE;
		setStatusCode(BAD_REQUEST);
	}
}

void	Request::parseBody()
{
	Bytes::const_iterator	copyUpTo;
	Bytes					data;
	int						fd;

	if (isPostMethod() || isPutMethod())
	{
		fd = open(bodyFilePath.c_str(), O_WRONLY | O_APPEND, 0700);
		if (fd < 0)
			setStatusCode(INTERNAL_ERROR);
		else
		{
			if (std::distance(newDataStart, newDataEnd) + body.size() < bodyLength)
				copyUpTo = newDataEnd;
			else
				copyUpTo = newDataStart + (bodyLength - body.size());
			data = Bytes(newDataStart, copyUpTo);
			if (write(fd, data.data(), data.size()) < 0)
				setStatusCode(INTERNAL_ERROR);
			body.insert(body.end(), newDataStart, copyUpTo);
			if (close(fd) < 0)
				setStatusCode(INTERNAL_ERROR);
		}
		newDataStart = copyUpTo;
	}
	if (body.size() >= bodyLength || !(isPostMethod() || isPutMethod()))
		requestStatus = COMPLETE;
}

void	Request::parseChunk()
{
	Bytes::const_iterator	it = searchPattern(newDataStart, newDataEnd, CRLF);
	Bytes					data;

	if (newDataStart == newDataEnd
		|| std::distance(newDataStart, newDataEnd) == 0
		|| it == newDataEnd)
		return ;
	// std::cout << "data start end:" << std::string(newDataStart, newDataEnd) << std::endl;
	// std::cout << "data start end dis:" << std::distance(newDataStart, newDataEnd) << std::endl;
	// std::cout << "chunk size str:" << std::string(newDataStart, it) << std::endl;
	if (expectedChunkSize == -1)
	{
		expectedChunkSize = hexToLL(std::string(newDataStart, it));
		if (expectedChunkSize == -1)
		{
			std::cout<<"chunk size error"<<std::endl;
			statusCode = Length_Required;
		}
		newDataStart = it + CRLF.size();
		return ;
	}
	// std::cout << "chunk size:" << expectedChunkSize << std::endl;
	// if (expectedChunkSize != (-1))
	// {
	// std::cout<<"size is ok"<<std::endl;
	// it += CRLF.size();
	// std::cout<<"distance:"<<std::distance(it, newDataEnd)<<std::endl;
	if (std::distance(newDataStart, newDataEnd) >= (long long)(expectedChunkSize + CRLF.size()))
	{
		// std::cout<<"got enough data"<<std::endl;
		if (searchPattern(newDataStart + expectedChunkSize, it + CRLF.size(), CRLF) == newDataEnd)
		{
			statusCode = BAD_REQUEST;
			requestStatus = COMPLETE;
		}
		else
		{
			// std::cout<<"writing chunk"<<std::endl;
			if (bodyFd == -1 && statusOK())
				bodyFd = open(bodyFilePath.c_str(), O_APPEND | O_CREAT | O_WRONLY, 0777);
			if (bodyFd < 0)
				statusCode = INTERNAL_ERROR;
			else
			{
				if (!expectedChunkSize)
				{
					requestStatus = COMPLETE;
					if (!body.empty() && write(bodyFd, body.data(), body.size()) < 0)
						statusCode = INTERNAL_ERROR;
					if (close(bodyFd) < 0)
						statusCode = INTERNAL_ERROR;
				}
				else
				{
					body.insert(body.end(), newDataStart, newDataStart + expectedChunkSize);
					// data = Bytes(newDataStart, newDataStart + expectedChunkSize);
					if (body.size() >= BUFFER_SIZE * 5)
					{
						if (write(bodyFd, body.data(), body.size()) < 0)
							statusCode = INTERNAL_ERROR;
						body.clear();
					}
				}
			}
			newDataStart = it + CRLF.size();
			expectedChunkSize = -1;
			// std::cout<<"writing chunk fin"<<std::endl;
		}
	}
	// }
	// else
	// {
	// 	statusCode = INTERNAL_ERROR;
	// 	newDataStart = it + CRLF.size();
	// }
}

void	Request::parseRequest()
{
	Bytes::const_iterator	it;

	while (!complete() &&
		((requestStatus != BODY && (it = searchPattern(newDataStart, newDataEnd, CRLF)) != newDataEnd)
			|| (requestStatus == BODY && newDataStart != newDataEnd)))
	{
		if (it == newDataStart && (newDataStart != newDataEnd) && requestStatus == HEADERS)
		{
			int	fd;

			requestStatus = BODY;
			std::srand(std::time(NULL));
			bodyFilePath = "tmp/" + toString(std::rand());
			while (fileExist(bodyFilePath))
				bodyFilePath = "tmp/" + toString(std::rand());
			fd = open(bodyFilePath.c_str(), O_CREAT | O_TRUNC, 0700);
			if (fd < 0 || close(fd) < 0)
				setStatusCode(INTERNAL_ERROR);
			if (switchToChunkMode())
			{
				std::cout << "change to chunk mode start" << std::endl;
				requestStatus = WAITING_CHUNK;
				std::cout << "change to chunk mode fin" << std::endl;
			}
			newDataStart = it + CRLF.size();
		}
		if ((requestStatus == BODY && headers.count("host") == 0))
			// || std::find(validHttpVersion.begin(), validHttpVersion.end(), method) == validHttpVersion.end())
		{
			setStatusCode(BAD_REQUEST);
			requestStatus = COMPLETE;
		}
		if (requestStatus == METHOD)
		{
			std::cout << "parse request line" << std::endl;
			parseRequestLine();
			newDataStart = it + CRLF.size();
		}
		else if (requestStatus == HEADERS)
		{
			std::cout << "parse header" << std::endl;
			parseRequestHeader();
			newDataStart = it + CRLF.size();
		}
		else if (requestStatus == BODY)
			parseBody();
		else if (requestStatus == WAITING_CHUNK)
			parseChunk();
	}
	if (!bodyFilePath.empty())
		bodyLength = fileSize(bodyFilePath);
	if (requestStatus == COMPLETE && std::find(validHttpVersion.begin(), validHttpVersion.end(), httpVer) == validHttpVersion.end())
		setStatusCode(BAD_REQUEST);
}

bool	Request::complete() const
{
	return requestStatus == COMPLETE;
}

bool	Request::statusOK() const
{
	return statusCode == HTTP_OK;
}

bool	Request::isMethod(std::string query) const
{
	return method == query;
}

bool	Request::isWaitingChunk() const
{
	return requestStatus == WAITING_CHUNK;
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
	std::cout << "\tVersion:" << httpVer << std::endl;
	std::cout << "\tHeader:" << std::endl;
	for (std::map<std::string, std::string>::const_iterator it = headers.begin(); it != headers.end(); ++it)
		std::cout << "\t\t" << it->first << " : " << it->second << std::endl;
	std::cout << std::endl;
}

void	Request::setDataStart(Bytes::const_iterator s)
{
	newDataStart = s;
}

void	Request::setDataEnd(Bytes::const_iterator e)
{
	newDataEnd = e;
}

void	Request::setStatusCode(int code)
{
	statusCode = code;
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

int	Request::getStatusCode() const
{
	return statusCode;
}

size_t	Request::getBodyLength() const
{
	return bodyLength;
}

std::string	Request::getBodyFilePath() const
{
	return bodyFilePath;
}

const std::vector<std::string>	&Request::getPaths() const
{
	return paths;
}

Bytes::const_iterator	Request::getDataStart() const
{
	return newDataStart;
}

Bytes::const_iterator	Request::getDataEnd() const
{
	return newDataEnd;
}
