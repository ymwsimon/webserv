/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   response.cpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mayeung <mayeung@student.42london.com>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/17 14:05:04 by mayeung           #+#    #+#             */
/*   Updated: 2026/01/09 17:30:39 by mayeung          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../include/response.hpp"

Response::Response(Service &ser, Request &req) : service(ser), request(req), statusCode(req.getStatusCode())
{
	std::string	filePathStr;

	matchLocation = ser.findMatchingRoute(req);
	resultType = NONE;
	pageStream = NULL;
	if (statusOK() && matchLocation)
	{
		filePathStr = mergeFullPath(matchLocation->getRootFolder(),
			req.getPaths(), req.getFileName());
		std::cout << "File path str: " << filePathStr << std::endl;
		if (fileExist(filePathStr) && !fileReadOK(filePathStr))
			setStatusCode(FORBIDDEN);
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
					setStatusCode(NOT_FOUND);
				if (resultPage.empty())
					setStatusCode(NOT_FOUND);
				std::cout << "result page size " << resultPage.size() << std::endl;
			}
		}
		else
			setStatusCode(NOT_FOUND);
	}
	if (resultPage.empty() && matchLocation && !resourcePath.empty() && !((matchLocation->findCGIExecutable(extractFileExt(resourcePath))).empty()))
	{
		std::cout << "ext for file: " << extractFileExt(resourcePath) << std::endl;
		std::cout << "cgi?: " << matchLocation->hasCGIConfig() << std::endl;
		std::cout << "is one of cgi?: " << matchLocation->isOneOfCGIConfig(resourcePath) << std::endl;
		std::cout << "cgi exe path: " << matchLocation->findCGIExecutable(extractFileExt(resourcePath)) << std::endl;
		Bytes	cgiRes;
		if (!((matchLocation->findCGIExecutable(extractFileExt(resourcePath))).empty()))
			cgiRes = exeCGI(matchLocation->findCGIExecutable(extractFileExt(resourcePath)));
		std::cout << "cgi res size: " << cgiRes.size() << std::endl;
		std::cout << "cgi res content: ";
		// printBytes(cgiRes);
		std::cout << std::endl;
		resultPage = convertCGIResToResponse(cgiRes);
	}
	if (resultPage.empty())
	{
		if (!resourcePath.empty())
		{
			pageStream = new std::ifstream(resourcePath.c_str(), std::ios_base::in);
			if (!pageStream->good())
			{
				std::cout << "can't open " << resourcePath << " as file\n";
				setStatusCode(NOT_FOUND);
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

Bytes	Response::convertCGIResToResponse(const Bytes &cgiRes)
{
	Bytes								res;
	Bytes::const_iterator				start = cgiRes.begin();
	Bytes::const_iterator				crlfPos;
	Bytes::const_iterator				colonPos;
	std::map<std::string, std::string>	headers;
	std::string							fieldName;
	std::string							fieldValue;
	std::string							line;

	while ((crlfPos = searchPattern(start, cgiRes.end(), CRLF)) != cgiRes.end())
	{
		if (start == crlfPos)
			break ;
		if ((colonPos = searchPattern(start, crlfPos, COLON)) == crlfPos)
		{
			std::cout << "no colon found\n";
			setStatusCode(INTERNAL_ERROR);
			return res;
		}
		fieldName = bytesToString(start, colonPos);
		std::transform(fieldName.begin(), fieldName.end(), fieldName.begin(), ::tolower);
		std::cout << "field name: " << fieldName << std::endl;
		if (headers.count(fieldName) > 0)
		{
			std::cout << "duplicate field name\n";
			setStatusCode(INTERNAL_ERROR);
			return res;
		}
		fieldValue = bytesToString(colonPos + 1, crlfPos);
		headers.insert(std::make_pair(fieldName, fieldValue));
		start = crlfPos + 2;
	}
	if (headers.empty() || headers.count(CONTENTTYPE) == 0)
	// if (headers.empty())
	{
		std::cout << "empty header\n";
		setStatusCode(INTERNAL_ERROR);
		return res;
	}
	if (headers.count("status") > 0 && headers.at("status") != "200")
	{
		// headers.at("status") >> statusCode;
		statusCode = toInt(headers.at("status"));
		if (statusCode == -1)
			setStatusCode(INTERNAL_ERROR);
		std::cout << "status is: " << headers.at("status") << std::endl;
		return res;
	}
	line = genHttpResponseLine(200);
	appendBytes(res, line);
	for (std::map<std::string, std::string>::iterator headerIt = headers.begin(); headerIt != headers.end(); ++ headerIt)
	{
		fieldName = headerIt->first;
		fieldValue = headerIt->second;
		line = genHttpHeader(fieldName, fieldValue);
		appendBytes(res, line);
	}
	int	size = std::distance(crlfPos + 2, cgiRes.end());
	line = genHttpHeader(CONTENTLENGTH, toString(size));
	appendBytes(res, line);
	appendBytes(res, CRLFStr);
	appendBytes(res, crlfPos + 2, cgiRes.end());
	return res;
}

Bytes	Response::exeCGI(std::string exe)
{
	Bytes		res;
	const char	*args[3];
	const char	*envs[6];
	int			pipeFd[2];
	int			pid;

	if (pipe(pipeFd) < 0)
	{
		std::cout << "error creating pipe\n";
		setStatusCode(INTERNAL_ERROR);
		return res;
	}
	pid = fork();
	if (pid < 0)
	{
		std::cout << "error forking\n";
		setStatusCode(INTERNAL_ERROR);
		return res;
	}
	if (!pid)
	{
		std::string	scriptFileName = "SCRIPT_FILENAME=";
		scriptFileName += resourcePath;
		resourcePath = "/home/user/cpp/webserv/data/www/test.php";
		close(pipeFd[0]);
		dup2(pipeFd[1], STDOUT_FILENO);
		args[0] = exe.c_str();
		// args[1] = resourcePath.c_str();
		args[1] = NULL;
		args[2] = NULL;
		// envs[0] = "SCRIPT_FILENAME=/home/user/cpp/webserv/data/www/test.php";
		envs[0] = scriptFileName.c_str();
		// envs[1] = NULL;
		// envs[1] = "SCRIPT_NAME=/home/user/cpp/webserv/data/www/test.php";
		envs[1] = "SERVER_PROTOCOL=HTTP/1.1";
		envs[2] = "REQUEST_METHOD=GET";
		envs[3] = "REDIRECT_STATUS=200";
		envs[4] = NULL;
		std::cerr << "exe par: " << args[0] << " " << args[1] << std::endl;
		std::cerr << "going to execute..\n";
		execve(exe.c_str(), (char **)args, (char **)envs);
		std::cerr << "error executing\n";
	}
	else
	{
		int	status;

		close(pipeFd[1]);
		std::cout << "waiting child\n";
		if (waitpid(pid, &status, WUNTRACED | WNOHANG) < 0)
			std::cout << "error waiting\n";
		std::cout << "finish waiting child\n";
		unsigned char	buf[BUFFER_SIZE];
		int				readSize;

		while ((readSize = read(pipeFd[0], buf, BUFFER_SIZE)) > 0)
			appendBuf(res, buf, readSize);
		close(pipeFd[0]);
	}
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
