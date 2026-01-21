/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   response.cpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mayeung <mayeung@student.42london.com>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/17 14:05:04 by mayeung           #+#    #+#             */
/*   Updated: 2026/01/21 17:16:42 by mayeung          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../include/response.hpp"

Response::Response(Service &ser, Request &req) : service(ser), request(req), statusCode(req.getStatusCode())
{
	resultType = NONE;
	cgiStage = INIT;
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
		matchLocation = right.matchLocation;
		resourcePath = right.resourcePath;
		pageStream = right.pageStream;
		cgiResFd = right.cgiResFd;
		cgiStage = right.cgiStage;
		resultPage = right.resultPage;
		cgiRes = right.cgiRes;
		statusCode = right.statusCode;
		resultType = right.resultType;
		cgiPid = right.cgiPid;
		pipeFd[0] = right.pipeFd[0];
		pipeFd[1] = right.pipeFd[1];
		cgiStartTime = right.cgiStartTime;
	}
	return *this;
}

bool	Response::statusOK() const
{
	return statusCode == 200;
}

bool	Response::isNoneType() const
{
	return resultType == NONE;
}

bool	Response::isCGI() const
{
	return resultType == CGI_EXE;
}

bool	Response::isINITStage() const
{
	return cgiStage == INIT;
}

bool	Response::isAddFdStage() const
{
	return cgiStage == ADD_FD_POLL;
}

bool	Response::isWaitingStage() const
{
	return cgiStage == WAITING_CGI;
}

bool	Response::isFinishWaitingStage() const
{
	return cgiStage == FINISH_WAITING;
}

int	Response::getStatusCode() const
{
	return statusCode;
}

const std::ifstream	*Response::getPageStream() const
{
	return pageStream;
}

int	Response::getResultType() const
{
	return resultType;
}

int	Response::getCgiResFd() const
{
	return cgiResFd;
}

int	Response::getCgiStage() const
{
	return cgiStage;
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
	std::cout << "Status code: " << statusCode << std::endl;
}

Bytes	Response::getPageStreamResponse()
{
	unsigned char		buf[BUFFER_SIZE];
	Bytes				res;
	std::string			head;
	int					fd;
	int					readSize;

	fd = open(resourcePath.c_str(), O_RDONLY);
	if (fd == -1)
	{
		setStatusCodeResType(INTERNAL_ERROR, ERR_PAGE);
		return res;
	}
	while ((readSize = read(fd, buf, BUFFER_SIZE)) > 0)
		appendBuf(res, buf, readSize);
	if (close(fd) < 0)
	{
		setStatusCodeResType(INTERNAL_ERROR, ERR_PAGE);
		return res;
	}
	if (readSize < 0)
	{
		setStatusCodeResType(INTERNAL_ERROR, ERR_PAGE);
		return res;
	}
	head = genHttpResponseLine(statusCode);
	head += genHttpHeader(CONTENTTYPE, getMediaType(extractFileExt(resourcePath)));
	head += genHttpHeader(CONTENTLENGTH, toString(res.size()));
	head += CRLFStr;
	res.insert(res.begin(), head.begin(), head.end());
	return res;
}

bool	Response::openPageStream()
{
	try
	{
		pageStream = new std::ifstream(resourcePath.c_str(), std::ios_base::in);
	}
	catch (std::exception &e)
	{
		setStatusCodeResType(INTERNAL_ERROR, ERR_PAGE);
		return false;
	}
	if (!pageStream->good())
	{
		std::cout << "can't open " << resourcePath << " as file\n";
		setStatusCodeResType(NOT_FOUND, ERR_PAGE);
		pageStream->close();
		delete pageStream;
		pageStream = NULL;
		return false;
	}
	return true;
}

Bytes	Response::convertCGIResToResponse(const Bytes &cgiRes)
{
	Bytes								res;
	Bytes::const_iterator				crlfPos;
	std::map<std::string, std::string>	headers;
	int									size;

	extractHeader(cgiRes, headers, crlfPos);
	if (headers.empty() || headers.count(CONTENTTYPE) == 0)
	{
		setStatusCodeResType(INTERNAL_ERROR, ERR_PAGE);
		return res;
	}
	if (headers.count("status") > 0 && headers.at("status") != "200")
	{
		statusCode = toInt(headers.at("status"));
		if (statusCode == -1)
			setStatusCodeResType(INTERNAL_ERROR, ERR_PAGE);
		return res;
	}
	appendBytes(res, genHttpResponseLine(200));
	for (std::map<std::string, std::string>::iterator headerIt = headers.begin();
		headerIt != headers.end(); ++headerIt)
		appendBytes(res, genHttpHeader(headerIt->first, headerIt->second));
	size = std::distance(crlfPos + CRLF.size(), cgiRes.end());
	appendBytes(res, genHttpHeader(CONTENTLENGTH, toString(size)));
	appendBytes(res, CRLFStr);
	appendBytes(res, crlfPos + CRLF.size(), cgiRes.end());
	return res;
}

void	Response::extractHeader(const Bytes &cgiRes, std::map<std::string, std::string> &headers,
	Bytes::const_iterator &crlfPos)
{
	Bytes::const_iterator				start = cgiRes.begin();
	Bytes::const_iterator				colonPos;
	std::string							fieldName;
	std::string							fieldValue;

	while ((crlfPos = searchPattern(start, cgiRes.end(), CRLF)) != cgiRes.end())
	{
		if (start == crlfPos)
			break ;
		if ((colonPos = searchPattern(start, crlfPos, COLON)) == crlfPos)
		{
			std::cout << "no colon found\n";
			setStatusCodeResType(INTERNAL_ERROR, ERR_PAGE);
			return ;
		}
		fieldName = bytesToString(start, colonPos);
		std::transform(fieldName.begin(), fieldName.end(), fieldName.begin(), ::tolower);
		if (headers.count(fieldName) > 0)
		{
			std::cout << "duplicate field name\n";
			setStatusCodeResType(INTERNAL_ERROR, ERR_PAGE);
			return ;
		}
		fieldValue = bytesToString(colonPos + 1, crlfPos);
		headers.insert(std::make_pair(fieldName, fieldValue));
		start = crlfPos + 2;
	}
}

void	Response::exeCGI(std::string exe)
{
	std::vector<std::string>	strs(8);
	std::vector<char *>			args(2);
	std::vector<char *>			env(9);
	int							fd;

	if (exe.empty())
		return ;
	if (pipe(pipeFd) < 0)
	{
		setStatusCodeResType(INTERNAL_ERROR, ERR_PAGE);
		return ;
	}
	cgiPid = fork();
	if (cgiPid < 0)
	{
		close(pipeFd[0]);
		close(pipeFd[1]);
		setStatusCodeResType(INTERNAL_ERROR, ERR_PAGE);
		return ;
	}
	if (!cgiPid)
	{
		prepareArgEnv(exe, strs, args, env);
		close(pipeFd[0]);
		fd = open(toString(pipeFd[0]).c_str(), O_WRONLY | O_CREAT | O_TRUNC, 0700);
		std::cout << "write fd for cgi input " << fd << std::endl;
		if (fd < 0)
			std::exit(1);
		if (write(fd, request.getBody().data(), request.getBody().size()) == -1)
			std::exit(1);
		if (close(fd) < 0)
			std::exit(1);
		fd = open(toString(pipeFd[0]).c_str(), O_RDONLY, 0700);
		std::cout << "read fd for cgi input " << fd << std::endl;
		if (fd < 0)
			std::exit(1);
		std::cout << "fd for cgi input " << fd << std::endl;
		if (dup2(pipeFd[1], STDOUT_FILENO) < 0)
			std::exit(1);
		if (dup2(fd, STDIN_FILENO) < 0)
			std::exit(1);
		if (close(fd) < 0)
		{
			std::cout << "can't close fd " << fd << " for cgi input"<< std::endl;
			std::exit(1);
		}
		execve(exe.c_str(), (char *const*)args.data(), (char *const*)env.data());
		std::exit(1);
	}
	else
	{
		cgiParent(-1);
	}
}

bool	Response::cgiParent(int evt)
{
	int				status;
	pid_t			waitRes;
	unsigned char	buf[BUFFER_SIZE];
	int				readSize = 0;

	if (cgiStage == INIT)
	{
		std::cout << "cgi init\n";
		close(pipeFd[1]);
		cgiStartTime = std::time(NULL);
		cgiResFd = pipeFd[0];
		cgiStage = ADD_FD_POLL;
	}
	else if (cgiStage == WAITING_CGI || cgiStage == ADD_FD_POLL)
	{
		if (cgiStage == ADD_FD_POLL)
			cgiStage = WAITING_CGI;
		if (evt == EXTRACT_PIPE && (readSize = read(pipeFd[0], buf, BUFFER_SIZE)) > 0)
		{
			std::cout << "extracting data from cgi pipe" << std::endl;
			appendBuf(cgiRes, buf, readSize);
			cgiStartTime = std::time(NULL);
		}
		if (readSize < 0)
		{
			setStatusCodeResType(INTERNAL_ERROR, ERR_PAGE);
			cgiRes.clear();
			cgiStage = FINISH_WAITING;
			resultType = ERR_PAGE;
			resultPage = stringToBytes(genHttpResponse(statusCode));
			return false;
		}
		waitRes = waitpid(cgiPid, &status, WUNTRACED | WNOHANG);
		if (waitRes < 0)
		{
			setStatusCodeResType(INTERNAL_ERROR, ERR_PAGE);
			cgiStage = FINISH_WAITING;
			resultType = ERR_PAGE;
			resultPage = stringToBytes(genHttpResponse(statusCode));
			return false;
		}
		if (!waitRes && std::difftime(std::time(NULL), cgiStartTime) > cgiWaitTime)
		{
			kill(cgiPid, SIGKILL);
			setStatusCodeResType(INTERNAL_ERROR, ERR_PAGE);
			std::cout << "time to kill cgiPid: " << cgiPid << std::endl;
		}
		if (waitRes == cgiPid || (!waitRes && std::difftime(std::time(NULL), cgiStartTime) > cgiWaitTime))
		{
			waitpid(cgiPid, &status, 0);
			cgiStage = FINISH_WAITING;
			std::cout << "cgi res size: " << cgiRes.size() << std::endl;
			std::cout << "cgi res content: ";
			// printBytes(cgiRes);
			std::cout << std::endl;
			resultPage = convertCGIResToResponse(cgiRes);
			if (resultPage.empty())
			{
				resultType = ERR_PAGE;
				resultPage = stringToBytes(genHttpResponse(statusCode));
			}
		}
	}
	return true;
}

void	Response::cgiExtractResult(Bytes &res, int *pipeFd)
{
	unsigned char	buf[BUFFER_SIZE];
	int				readSize;

	while ((readSize = read(pipeFd[0], buf, BUFFER_SIZE)) > 0)
		appendBuf(res, buf, readSize);
	close(pipeFd[0]);
	if (readSize < 0)
	{
		setStatusCodeResType(INTERNAL_ERROR, ERR_PAGE);
		res.clear();
	}
}

void	Response::prepareArgEnv(std::string exe, std::vector<std::string> &strs,
	std::vector<char *> &args, std::vector<char *> &env)
{
	strs[0] = "SCRIPT_FILENAME=";
	strs[1] = "PATH_INFO=";
	strs[2] = "SERVER_PROTOCOL=";
	strs[3] = "REQUEST_METHOD=";
	strs[4] = "REDIRECT_STATUS=";
	strs[5] = "CONTENT_LENGTH=";
	strs[6] = "CONTENT_TYPE=";
	strs[7] = "GATEWAY_INTERFACE=CGI/1.1";
	strs[0] += resourcePath;
	strs[1] += "/";
	strs[2] += request.getHttpVer();
	strs[3] += request.getMethod();
	strs[4] += "200";
	strs[5] += toString(request.getBodyLength());
	if (request.getHeaders().count(CONTENTTYPE) > 0)
		strs[6] += request.getHeaders().at(CONTENTTYPE);
	else
		strs[6] = "";
	args[0] = (char *) exe.c_str();
	args[1] = NULL;
	env[0] = (char *) strs[0].c_str();
	env[1] = (char *) strs[1].c_str();
	env[2] = (char *) strs[2].c_str();
	env[3] = (char *) strs[3].c_str();
	env[4] = (char *) strs[4].c_str();
	env[5] = (char *) strs[5].c_str();
	env[6] = (char *) strs[6].c_str();
	env[7] = (char *) strs[7].c_str();
	env[8] = NULL;
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

void	Response::setStatusCodeResType(int code, int rType)
{
	statusCode = code;
	resultType = rType;
}

void	Response::setCgiStage(int stage)
{
	cgiStage = stage;
}

void	Response::handleCGIExe()
{
	std::string	exeBin;

	if (isRegularFile(resourcePath) && !fileExeOK(resourcePath))
		resultType = FILE;
	else
	{
		std::cout << "resource path in cgi: " << resourcePath << std::endl;
		// std::cout << "ext for file: " << extractFileExt(resourcePath) << std::endl;
		// std::cout << "cgi?: " << matchLocation->hasCGIConfig() << std::endl;
		// std::cout << "is one of cgi?: " << matchLocation->isOneOfCGIConfig(resourcePath) << std::endl;
		// std::cout << "cgi exe path: " << matchLocation->findCGIExecutable(extractFileExt(resourcePath)) << std::endl;
		exeBin = matchLocation->findCGIExecutable(extractFileExt(resourcePath));
		if (exeBin.empty() && fileExeOK(resourcePath))
			exeBin = defaultExeBin;
		exeCGI(exeBin);
	}
}

void	Response::determineResType(void)
{
	std::string	filePathStr;
	
	filePathStr = mergeFullPath(matchLocation->getRootFolder(),
		request.getPaths(), matchLocation->hasCGIConfig());
	logMessage(std::cout, "FilePath str: " + filePathStr);
	if (matchLocation->hasCGIConfig())
		resultType = CGI_EXE;
	if (isDir(filePathStr))
	{
		if (*filePathStr.rbegin() != '/')
			filePathStr.push_back('/');
		logMessage(std::cout, "dir: " + filePathStr);
		resourcePath = matchLocation->findValidIndexPage(filePathStr);
		logMessage(std::cout, "Resource path str: " + resourcePath);
		if (resourcePath.empty())
		{
			logMessage(std::cout, "index page not found");
			resourcePath = filePathStr;
			if (matchLocation->getAutoIndex())
				(logMessage(std::cout, "list folder content.." + resourcePath), resultType = LIST_FOLDER);
			else
				setStatusCodeResType(NOT_FOUND, ERR_PAGE);
		}
	}
	if (isRegularFile(filePathStr))
		resourcePath = filePathStr;
	if (resultType == NONE)
		resultType = FILE;
}

void	Response::processResponse()
{
	matchLocation = service.findMatchingRoute(request);
	if (!statusOK())
		resultType = ERR_PAGE;
	if (statusOK() && !matchLocation)
		(logMessage(std::cout, "no route match"), setStatusCodeResType(NOT_FOUND, ERR_PAGE));
	if (statusOK() && matchLocation && !matchLocation->isMethodAllowed(request.getMethod()))
		(logMessage(std::cout, "method not allowed"), setStatusCodeResType(FORBIDDEN, ERR_PAGE));
	if (statusOK() && matchLocation)
		determineResType();
	if (statusOK() && !fileExist(resourcePath))
		setStatusCodeResType(NOT_FOUND, ERR_PAGE);
	if (statusOK() && fileExist(resourcePath) && !fileReadOK(resourcePath))
		setStatusCodeResType(FORBIDDEN, ERR_PAGE);
	if (statusOK() && resultType == LIST_FOLDER)
		resultPage = matchLocation->generateIndexPages(resourcePath,
			mergeFullPath("", request.getPaths(), false));
	if (statusOK() && resultType == CGI_EXE)
		handleCGIExe();
	if (statusOK() && resultType == FILE)
		resultPage = getPageStreamResponse();
	if (resultType == ERR_PAGE)
		resultPage = stringToBytes(genHttpResponse(statusCode));
}
