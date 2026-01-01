/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   response.cpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mayeung <mayeung@student.42london.com>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/17 14:05:04 by mayeung           #+#    #+#             */
/*   Updated: 2026/01/01 19:16:27 by mayeung          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../include/response.hpp"

Response::Response(Service &ser, Request &req) : service(ser), request(req), statusCode(req.getStatusCode())
{
	std::string	filePathStr;

	matchLocation = ser.findMatchingRoute(req);
	pageStream = NULL;
	if (statusOK() && matchLocation)
	{
		filePathStr = mergeFullPath(matchLocation->getRootFolder(),
			req.getPaths(), req.getFileName());
		std::cout << "File path str: " << filePathStr << std::endl;
		if (fileExist(filePathStr) && !fileReadOK(filePathStr))
			statusCode = 403;
		else if (isRegularFile(filePathStr))
		{
			std::cout << "opening " << filePathStr << " as file\n";
			resourcePath = filePathStr;
		}
		else if (isDir(filePathStr))
		{
			if (!req.getFileName().empty())
				filePathStr.append("/");
			std::cout << "dir\n" << filePathStr << std::endl;
			resourcePath = matchLocation->findValidIndexPage(filePathStr);
			std::cout << "resourcePath: " << resourcePath << std::endl;
			if (resourcePath.empty())
			{
				std::cout << "index page not found\n";
				std::cout << "list folder content.." << filePathStr << std::endl;
				if (matchLocation->getAutoIndex())
					resultPage = matchLocation->generateIndexPages(filePathStr,
						mergeFullPath("", req.getPaths(), req.getFileName()));
				else if (statusOK())
					setStatusCode(404);
				if (resultPage.empty())
					setStatusCode(404);
				std::cout << "result page size " << resultPage.size() << std::endl;
			}
		}
		else
			statusCode = 404;
	}
	if (resultPage.empty())
	{
		if (!resourcePath.empty())
		{
			std::cout << "ext for file " << extractFileExt(resourcePath) << std::endl;
			std::cout << "cgi path for file " << matchLocation->findCGIExecutable(extractFileExt(resourcePath)) << std::endl;
			pageStream = new std::ifstream(resourcePath.c_str(), std::ios_base::in);
			if (!pageStream->good())
			{
				std::cout << "can't open " << resourcePath << " as file\n";
				setStatusCode(404);
				pageStream->close();
				delete pageStream;
				pageStream = NULL;
			}
		}
		if (pageStream)
			resultPage = getPageStreamResponse();
		else
			resultPage = stringToBytes(genHttpResponse(statusCode));
	}
}

Response::Response(const Response &right) : service(right.service), request(right.request)
{
	*this = right;
}

Response::~Response()
{

}

Response	&Response::operator=(const Response &right)
{
	if (this != &right)
	{
		service = right.service;
		request = right.request;
		pageStream = right.pageStream;
		matchLocation = right.matchLocation;
		statusCode = right.statusCode;
		resultPage = right.resultPage;
	}
	return *this;
}

bool	Response::statusOK() const
{
	return statusCode == 200;
}

int	Response::getStatusCode() const
{
	return statusCode;
}

const std::ifstream	*Response::getPageStream() const
{
	return pageStream;
}

const Bytes	&Response::getResultPage() const
{
	return resultPage;
}

const Location	*Response::getMatchLocation() const
{
	return matchLocation;
}

const std::string	Response::getResourcePath() const
{
	return resourcePath;
}

void	Response::printResponse() const
{
	std::cout << "Response: " << std::endl;
	if (!matchLocation)
		std::cout << "No match location" << std::endl;
	std::cout << "Error code: " << statusCode << std::endl;
}

Bytes	Response::getPageStreamResponse()
{
	Bytes				buf(BUFFER_SIZE);
	Bytes				res;
	std::string			head;
	std::stringstream	strStream;

	pageStream->read((char *)buf.data(), BUFFER_SIZE);
	pageStream->close();
	buf.resize(pageStream->gcount());
	head = genHttpResponseLine(statusCode);
	head += genHttpHeader("Content-Type", getMediaType("html"));
	head += genHttpHeader("Content-Length", toString(buf.size()));
	head += CRLFStr;
	res.insert(res.end(), head.begin(), head.end());
	res.insert(res.end(), buf.begin(), buf.begin() + buf.size());
	delete pageStream;
	pageStream = NULL;
	return res;
}

void	Response::setStatusCode(int code)
{
	statusCode = code;
}

void	Response::setMatchLocation(const Location *loc)
{
	matchLocation = loc;
}

void	Response::setResourcePath(const std::string path)
{
	resourcePath = path;
}
