/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   request.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mayeung <mayeung@student.42london.com>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/14 23:12:55 by mayeung           #+#    #+#             */
/*   Updated: 2026/03/05 11:29:02 by mayeung          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../include/request.hpp"

std::string Request::valMet[4] = {"POST", "GET", "DELETE", "HEAD"};
std::vector<std::string>	Request::validMethod = std::vector<std::string>(Request::valMet, Request::valMet + 4);
std::string Request::valVer[1] = {"HTTP/1.1"};
std::vector<std::string>	Request::validHttpVersion = std::vector<std::string>(Request::valVer, Request::valVer + 1);

Request::Request(Bytes::const_iterator start, Bytes::const_iterator end, Bytes &inData)
	: newDataStart(start), newDataEnd(end), incomingData(inData)
{
	requestStatus = METHOD;
	statusCode = HTTP_OK;
	bodyLength = 0;
	expectedChunkSize = -1;
	bodyFd = -1;
	updated = true;
	chunkMode = false;
}

Request::Request(const Request &right) : incomingData(right.incomingData)
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
		incomingData = right.incomingData;
		statusCode = right.statusCode;
		requestStatus = right.requestStatus;
		bodyLength = right.bodyLength;
		bodyFilePath = right.bodyFilePath;
		expectedChunkSize = right.expectedChunkSize;
		bodyFd = right.bodyFd;
		updated = right.updated;
		chunkMode = right.chunkMode;
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
	if (std::find(validMethod.begin(), validMethod.end(), method) == validMethod.end())
		setStatusCode(Not_Implemented);
	if (method.empty()
		|| route.empty() || route[0] != '/'
		|| std::find(validHttpVersion.begin(), validHttpVersion.end(), httpVer) == validHttpVersion.end()
		|| httpVer.empty())
		setStatusCode(BAD_REQUEST);
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

bool	Request::isHeadMethod() const
{
	return method == "HEAD";
}

bool	Request::isChunkMode() const
{
	return switchToChunkMode();
}

bool	Request::isUpdated() const
{
	return updated;
}

bool	Request::readyparseChunkLength() const
{
	return requestStatus == CHUNK_LENGTH
		&& searchPattern(incomingData, CRLF) != incomingData.end();
}

bool	Request::readyparseChunkData() const
{
	return requestStatus == CHUNK_DATA
		&& incomingData.size() >= (size_t)(expectedChunkSize + CRLF.size());
}

bool	Request::readyparseBody() const
{
	return requestStatus == BODY;
}

bool	Request::switchToChunkMode() const
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
	Bytes::iterator	copyUpTo;

	if (isPostMethod() || isPutMethod())
	{
		copyUpTo = incomingData.end();
		if (incomingData.size() + body.size() >= bodyLength)
			copyUpTo = incomingData.begin() + (bodyLength - body.size());
		newDataStart = copyUpTo;
		if (searchPattern(copyUpTo, incomingData.end(), CRLF) != incomingData.end()
			&& searchPattern(copyUpTo, incomingData.end(), CRLF) == copyUpTo)
			newDataStart = copyUpTo + CRLF.size();
		if (!isChunkMode())
		{
			Bytes	data;
			int		fd;

			fd = open(bodyFilePath.c_str(), O_WRONLY | O_APPEND, 0777);
			if (fd < 0)
				setStatusCode(INTERNAL_ERROR);
			else
			{
				data = Bytes(incomingData.begin(), copyUpTo);
				if (write(fd, data.data(), data.size()) < 0)
					setStatusCode(INTERNAL_ERROR);
				if (close(fd) < 0)
					setStatusCode(INTERNAL_ERROR);
			}
		}
	}
	if ((fileSize(bodyFilePath) >= (long long int)bodyLength)
		|| !(isPostMethod() || isPutMethod()))
	{
		requestStatus = COMPLETE;
	}
}

void	Request::parseChunkLength()
{
	Bytes::const_iterator	it = searchPattern(incomingData.begin(), incomingData.end(), CRLF);

	if (it == incomingData.begin())
		return ;
	expectedChunkSize = hexToLL(std::string(static_cast<Bytes::const_iterator>(incomingData.begin()), it));
	if (expectedChunkSize == -1)
	{
		statusCode = Length_Required;
		requestStatus = COMPLETE;
	}
	else
	{
		newDataStart = it + CRLF.size();
		requestStatus = CHUNK_DATA;
	}
}

void	Request::parseChunkData()
{
	if (incomingData.size() >= expectedChunkSize + CRLF.size())
	{
		if (searchPattern(incomingData.begin() + expectedChunkSize,
			incomingData.begin() + expectedChunkSize + CRLF.size(), CRLF)
				== incomingData.begin() + expectedChunkSize + CRLF.size())
		{
			statusCode = BAD_REQUEST;
			requestStatus = COMPLETE;
		}
		else
		{
			bodyLength += expectedChunkSize;
			newDataStart = incomingData.begin() + expectedChunkSize + CRLF.size();
			if (!expectedChunkSize)
			{
				requestStatus = COMPLETE;
			}
			else
			{
				body.insert(body.end(), incomingData.begin(), incomingData.begin() + expectedChunkSize);
				requestStatus = CHUNK_LENGTH;
				expectedChunkSize = -1;
			}
		}
	}
}

void	Request::parseRequest()
{
	Bytes::const_iterator	it;

	if (!complete() &&
		((requestStatus < BODY && (it = searchPattern(incomingData.begin(), incomingData.end(), CRLF)) != incomingData.end())
			|| readyparseBody()
			|| readyparseChunkLength()
			|| readyparseChunkData()))
	{
		updated = true;
		if (it == incomingData.begin() && incomingData.size() > 0 && requestStatus == HEADERS)
		{
			if (switchToChunkMode())
			{
				chunkMode = true;
				requestStatus = CHUNK_LENGTH;
				if (headers.count(CONTENT_LENGTH) > 0)
					headers.erase(CONTENT_LENGTH);
			}
			else if (bodyFilePath.empty())
			{
				int			fd;
				std::string	tmpPath = "tmp/";
			
				requestStatus = BODY;
				std::srand(std::time(NULL));
				if (!fileExist(tmpPath))
					tmpPath = "";
				bodyFilePath = tmpPath + toString(std::rand());
				while (fileExist(bodyFilePath))
					bodyFilePath = tmpPath + toString(std::rand());
				(void)fd;
				fd = open(bodyFilePath.c_str(), O_CREAT | O_TRUNC, 0777);
				if (fd < 0 || close(fd) < 0)
					setStatusCode(INTERNAL_ERROR);
			}
			newDataStart = it + CRLF.size();
		}
		if ((requestStatus == BODY && headers.count("host") == 0))
		{
			setStatusCode(BAD_REQUEST);
			requestStatus = COMPLETE;
		}
		if (requestStatus == METHOD)
		{
			parseRequestLine();
			newDataStart = it + CRLF.size();
		}
		else if (requestStatus == HEADERS)
		{
			parseRequestHeader();
			newDataStart = it + CRLF.size();
		}
		else if (requestStatus == BODY)
			parseBody();
		else if (requestStatus == CHUNK_LENGTH)
			parseChunkLength();
		else if (requestStatus == CHUNK_DATA)
			parseChunkData();
	}
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
	return requestStatus == CHUNK_LENGTH || requestStatus == CHUNK_DATA;
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

void	Request::setIncomingData(Bytes &inData)
{
	incomingData = inData;
}

void	Request::setStatusCode(int code)
{
	statusCode = code;
}

void	Request::setUpdated(bool b)
{
	updated = b;
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

void	Request::clearBody()
{
	body.clear();
}

void	Request::removeNCharFromBody(size_t n)
{
	if (n <= body.size())
		body.erase(body.begin(), body.begin() + n);
	else
		body.clear();
}

void	Request::setToComplete()
{
	requestStatus = COMPLETE;
}
