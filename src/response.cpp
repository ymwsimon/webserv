/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   response.cpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mayeung <mayeung@student.42london.com>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/17 14:05:04 by mayeung           #+#    #+#             */
/*   Updated: 2026/03/02 12:05:46 by mayeung          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../include/response.hpp"

Response::Response(Service *ser, Request &req) : service(ser), request(req)
{
	statusCode = req.getStatusCode();
	init();
}

void	Response::init()
{
	paths = request.getPaths();
	route = request.getRoute();
	method = request.getMethod();
	resultType = NONE;
	cgiStage = INIT;
	matchLocation = NULL;
	if (!statusOK())
		resultType = ERR_PAGE;
	byteWritten = 0;
	byteExtracted = 0;
	byteConverted = 0;
	eraseLimit = 0;
	bodyFilePath = request.getBodyFilePath();
	cgiPid = 0;
	resultSent = false;
	cgiOutPipeDrained = false;
	allHeaderExtracted = false;
	endChunkAppended = false;
	fileAllRead = false;
	resultPage.clear();
	cgiRes.clear();
	headers.clear();
	fileFd = -2;
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
		paths = right.paths;
		route = right.route;
		method = right.method;
		matchLocation = right.matchLocation;
		resourcePath = right.resourcePath;
		cgiOutFd = right.cgiOutFd;
		cgiInFd = right.cgiInFd;
		byteWritten = right.byteWritten;
		byteExtracted = right.byteExtracted;
		byteConverted = right.byteConverted;
		eraseLimit = right.eraseLimit;
		cgiStage = right.cgiStage;
		resultPage = right.resultPage;
		cgiRes = right.cgiRes;
		statusCode = right.statusCode;
		resultType = right.resultType;
		cgiPid = right.cgiPid;
		outPipeFd[0] = right.outPipeFd[0];
		outPipeFd[1] = right.outPipeFd[1];
		inPipeFd[0] = right.inPipeFd[0];
		inPipeFd[1] = right.inPipeFd[1];
		cgiLastActiveTime = right.cgiLastActiveTime;
		headers = right.headers;
		locationMatchLength = right.locationMatchLength;
		bodyFilePath = right.bodyFilePath;
		resultSent = right.resultSent;
		cgiOutPipeDrained = right.cgiOutPipeDrained;
		allHeaderExtracted = right.allHeaderExtracted;
		endChunkAppended = right.endChunkAppended;
		fileAllRead = right.fileAllRead;
		fileFd = right.fileFd;
	}
	return *this;
}

bool	Response::statusOK() const
{
	return (statusCode / 100) == 2;
}

bool	Response::isNoneType() const
{
	return resultType == NONE;
}

bool	Response::isFileType() const
{
	return resultType == FILE;
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
	return cgiStage == WAITING_CGI_BODY || cgiStage == WAITING_HEADER;
}

bool	Response::isFinishWaitingStage() const
{
	return cgiStage == FINISH_WAITING;
}

bool	Response::needCloseCgiInFd() const
{
	return isChunkMode() && request.complete() && request.getBody().size() == 0;
}

bool	Response::isChunkMode() const
{
	return request.isChunkMode();
}

bool	Response::gotEnoughChunkDataToSent() const
{
	return resultPage.size() >= TRANSFER_SIZE;
}

bool	Response::isResultPageEmpty() const
{
	return resultPage.empty();
}

bool	Response::isResultSent() const
{
	return resultSent;
}

bool	Response::isCgiOutPipeDrained() const
{
	return cgiOutPipeDrained;
}

bool	Response::isEndChunkAppended() const
{
	return endChunkAppended;
}

bool	Response::isHeadMethod() const
{
	return method == "HEAD";
}

bool	Response::isStatusCodeinCustomErrorPage() const
{
	return service->getServiceConfig().getErrorPages().count(statusCode) > 0;
}

bool	Response::isRedirectStatusCode() const
{
	return statusCode / 100 == 3;
}

bool	Response::isFileAllRead() const
{
	return fileAllRead;
}

int	Response::getStatusCode() const
{
	return statusCode;
}

int	Response::getResultType() const
{
	return resultType;
}

int	Response::getCgiOutFd() const
{
	return cgiOutFd;
}

int	Response::getCgiInFd() const
{
	return cgiInFd;
}

int	Response::getCgiStage() const
{
	return cgiStage;
}

const std::map<int, std::string>	&Response::getCustomErrorPage() const
{
	return service->getServiceConfig().getErrorPages();
}

const Bytes	&Response::getResultPage() const
{
	return resultPage;
}

const Bytes	&Response::getCgiRes() const
{
	return cgiRes;
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

void	Response::getFileResponse()
{
	std::string			head;
	int					readSize;

	if (fileFd == -2)
		fileFd = open(resourcePath.c_str(), O_RDONLY, 0700);
	if (fileFd < 0)
	{
		setStatusCodeResType(INTERNAL_ERROR, ERR_PAGE);
		fileAllRead = true;
		return ;
	}
	else if (!resultSent && resultPage.empty())
	{
		std::cout<<"append head"<<std::endl;
		genHttpResponseLine(statusCode, head);
		genHttpHeader(CONTENT_TYPE, getMediaType(extractFileExt(resourcePath)), head);
		genHttpHeader(CONTENT_LENGTH, toString(fileSize(resourcePath)), head);
		head.append(CRLFStr);
		resultPage.insert(resultPage.begin(), head.begin(), head.end());
	}
	if (!fileAllRead)
	{
		std::cout<<"read file"<<std::endl;
		readSize = read(fileFd, buf, BUFFER_SIZE);
		if (readSize > 0)
			appendBuf(resultPage, buf, readSize);
		else if (readSize == 0)
		{
			fileAllRead = true;
			if (close(fileFd) < 0)
			{
				if (!resultSent)
					setStatusCodeResType(INTERNAL_ERROR, ERR_PAGE);
				else
					statusCode = INTERNAL_ERROR;
			}
		}
		else if (readSize == - 1)
		{
			if (!resultSent)
				setStatusCodeResType(INTERNAL_ERROR, ERR_PAGE);
			else
				statusCode = INTERNAL_ERROR;
		}
	}
}

std::string	Response::getBodyFilePath() const
{
	return bodyFilePath;
}

size_t	Response::getByteWritten() const
{
	return byteWritten;
}

size_t	Response::getEraseLimit() const
{
	return eraseLimit;
}

pid_t	Response::getPid() const
{
	return cgiPid;
}

pid_t	Response::getWaitRes() const
{
	return waitRes;
}

int	Response::getWaitStatus() const
{
	return waitStatus;
}

bool	Response::convertCGIResToResponse()
{
	Bytes::const_iterator				crlfPos;
	extractHeader(crlfPos);
	if (headers.empty() || headers.count(CONTENT_TYPE) == 0)
	{
		std::cout<<"empty header"<<std::endl;
		setStatusCodeResType(INTERNAL_ERROR, ERR_PAGE);
		return false;
	}
	if (headers.count(STATUS) > 0)
	{
		std::string				statusStr = headers.at(STATUS);
		std::string::iterator	spIter = std::search(statusStr.begin(), statusStr.end(),
			SPACE.begin(), SPACE.end());
		// std::cout<<"status err"<<std::endl;
		std::cout<<"status: "<<statusStr <<std::endl;

		statusCode = toInt(std::string(statusStr.begin(), spIter));
		std::cout<<"status code: "<<statusCode <<std::endl;
		if (statusCode == -1)
			setStatusCodeResType(INTERNAL_ERROR, ERR_PAGE);
		// 	statusCode = INTERNAL_ERROR;
		// return false;
	}
	appendHeaderToResultPage(crlfPos);
	appendBytes(resultPage, crlfPos + CRLF.size(), cgiRes.end());
	return true;
}

void	Response::appendHeaderToResultPage(Bytes::const_iterator crlfPos)
{
	size_t	size;

	appendBytes(resultPage, genHttpResponseLine(statusCode));
	if (isChunkMode())
		headers.insert(std::make_pair(TRANSFER_ENDCODING, CHUNKED));
	else
	{
		size = cgiRes.size() - std::distance(static_cast<Bytes::const_iterator>(cgiRes.begin()),
			crlfPos + CRLF.size());
		headers.insert(std::make_pair(CONTENT_LENGTH, toString(size)));
	}
	for (std::map<std::string, std::string>::iterator headerIt = headers.begin();
		headerIt != headers.end(); ++headerIt)
		appendBytes(resultPage, genHttpHeader(headerIt->first, headerIt->second));
	appendBytes(resultPage, CRLFStr);
}

void	Response::extractHeader(Bytes::const_iterator &crlfPos)
{
	Bytes::const_iterator				start = cgiRes.begin();
	Bytes::const_iterator				colonPos;
	std::string							fieldName;
	std::string							fieldValue;

	while ((crlfPos = searchPattern(start, cgiRes.end(), CRLF)) != cgiRes.end())
	{
		if (start == crlfPos)
		{
			if (!headers.empty())
			{
				std::cout<<"end header parse for cgi Res"<<std::endl;
				if (isChunkMode())
				{
					removeCgiResUpTo(start + CRLF.size());
					appendHeaderToResultPage(crlfPos);
				}
				allHeaderExtracted = true;
			}
			else
			{
				std::cout<<"empty header"<<std::endl;
			}
			break ;
		}
		if ((colonPos = searchPattern(start, crlfPos, COLON)) == crlfPos)
		{
			std::cout << "no colon found" << std::endl;
			setStatusCodeResType(INTERNAL_ERROR, ERR_PAGE);
			return ;
		}
		fieldName = bytesToString(start, colonPos);
		std::transform(fieldName.begin(), fieldName.end(), fieldName.begin(), ::tolower);
		if (headers.count(fieldName) > 0)
		{
			std::cout << "duplicate field name" << std::endl;
			setStatusCodeResType(INTERNAL_ERROR, ERR_PAGE);
			return ;
		}
		fieldValue = bytesToString(colonPos + COLON.size(), crlfPos);
		trim(fieldValue);
		headers.insert(std::make_pair(fieldName, fieldValue));
		start = crlfPos + CRLF.size();
		if (isChunkMode())
		{
			removeCgiResUpTo(start);
			start = cgiRes.begin();
		}
	}
}

void	Response::appendBodyForChunkMode()
{
	size_t		size = std::min((size_t)CHUNK_SIZE, cgiRes.size());
	std::string	sizeStr = ulToHex(size);

	// std::cout<<"append size to body:"<<size<<std::endl;
	// std::cout<<"result page size before add to body:"<<resultPage.size()<<std::endl;
	resultPage.insert(resultPage.end(), sizeStr.begin(), sizeStr.end());
	resultPage.insert(resultPage.end(), CRLF.begin(), CRLF.end());
	resultPage.insert(resultPage.end(), cgiRes.begin(), cgiRes.begin() + size);
	resultPage.insert(resultPage.end(), CRLF.begin(), CRLF.end());
	// std::cout<<"result page size after add to body:"<<resultPage.size()<<std::endl;
	// std::cout<<"cgi res size before add to body:"<<cgiRes.size()<<std::endl;
	cgiRes.erase(cgiRes.begin(), cgiRes.begin() + size);
	// std::cout<<"cgi res size after add to body:"<<cgiRes.size()<<std::endl;
	byteConverted += size;
}

void	Response::startCgi()
{
	std::vector<std::string>			strs;
	std::vector<char *>					args;
	std::vector<char *>					env;
	std::map<std::string, std::string>	headersToAdd = request.getHeaders();
	std::map<std::string, std::string>	allHeaders;
	std::string							exe;

	exe = matchLocation->findCGIExecutable(extractFileExt(resourcePath));
	cgiExeBasePath = matchLocation->getRootFolder();
	if (exe.empty() && fileExeOK(resourcePath))
		exe = resourcePath;
	if (pipe(outPipeFd) < 0)
	{
		std::cout << "create pipe fail" << std::endl;
		setStatusCodeResType(INTERNAL_ERROR, ERR_PAGE);
		cgiStage = FINISH_WAITING;
		return ;
	}
	setToNonBlock(outPipeFd[0]);
	setToNonBlock(outPipeFd[1]);
	cgiOutFd = outPipeFd[0];
	if (pipe(inPipeFd) < 0)
	{
		close(outPipeFd[0]);
		close(outPipeFd[1]);
		std::cout << "create pipe fail" << std::endl;
		setStatusCodeResType(INTERNAL_ERROR, ERR_PAGE);
		cgiStage = FINISH_WAITING;
		return ;
	}
	setToNonBlock(inPipeFd[0]);
	setToNonBlock(inPipeFd[1]);
	cgiInFd = inPipeFd[1];
	cgiPid = fork();
	if (cgiPid < 0)
	{
		close(inPipeFd[0]);
		close(inPipeFd[1]);
		close(outPipeFd[0]);
		close(outPipeFd[1]);
		setStatusCodeResType(INTERNAL_ERROR, ERR_PAGE);
		cgiStage = FINISH_WAITING;
		return ;
	}
	if (!cgiPid)
	{
		addHttpPrefixToHeaders(headersToAdd, allHeaders);
		addCgiHeaders(allHeaders);
		mergeEnvStrs(allHeaders, strs);
		prepareArgEnv(exe, strs, args, env);
		if (close(outPipeFd[0]) < 0 || close(inPipeFd[1]) < 0)
			std::exit(1);
		if (dup2(outPipeFd[1], STDOUT_FILENO) < 0 || dup2(inPipeFd[0], STDIN_FILENO) < 0)
			std::exit(1);
		if (!request.isChunkMode())
		{
			int	fd;

			fd = open(bodyFilePath.c_str(), O_RDONLY, 0700);
			if (close(inPipeFd[0]) < 0
				|| fd < 0
				|| dup2(fd, STDIN_FILENO) < 0
				|| close(fd) < 0)
				std::exit(1);
		}
		if (!cgiExeBasePath.empty() && chdir(cgiExeBasePath.c_str()) < 0)
			std::exit(1);
		execve(exe.c_str(), (char *const*)args.data(), (char *const*)env.data());
		std::exit(1);
	}
	else
	{
		if (close(outPipeFd[1]) < 0 || close(inPipeFd[0]) < 0)
		{
			setStatusCodeResType(INTERNAL_ERROR, ERR_PAGE);
			cgiStage = FINISH_WAITING;
			return ;
		}
		if (!request.isChunkMode() && close(inPipeFd[1]) < 0)
		{
			setStatusCodeResType(INTERNAL_ERROR, ERR_PAGE);
			cgiStage = FINISH_WAITING;
			return ;
		}
		std::time(&cgiLastActiveTime);
		byteWritten = 0;
		byteExtracted = 0;
		byteConverted = 0;
		cgiStage = ADD_FD_POLL;
	}
}

ssize_t	Response::processCgi(int op)
{
	if (cgiStage == INIT)
		startCgi();
	else if (cgiStage == WAITING_HEADER || cgiStage == WAITING_CGI_BODY)
	{
		if ((waitRes = waitpid(cgiPid, &waitStatus, WUNTRACED | WNOHANG)) < 0)
		{
			if (!resultSent)
			{
				setStatusCodeResType(INTERNAL_ERROR, ERR_PAGE);
			}
			else
			{
				cgiStage = FINISH_WAITING;
			}
			return -1;
		}
		if ((!waitRes && std::difftime(std::time(NULL), cgiLastActiveTime) > cgiWaitTime))
			// || op == KILL_PROCESS)
		{
			std::cout << "time to kill cgiPid: " << cgiPid << std::endl;
			kill(cgiPid, SIGKILL);
			cgiStage = FINISH_WAITING;
			waitRes = waitpid(cgiPid, &waitStatus, WUNTRACED);
			if (cgiRes.empty() && !resultSent)
				setStatusCodeResType(INTERNAL_ERROR, ERR_PAGE);
			// if (isChunkMode())
			// {
			// 	std::cout <<"end chunk transfer"<<std::endl;
			// 	endChunkTransfer();
			// }
			std::cout << "finish kill cgiPid: " << cgiPid << std::endl;
		}
		if (waitRes == cgiPid)
		{
			if (WIFEXITED(waitStatus) && WEXITSTATUS(waitStatus) != 0)
			{
				if (!resultSent)
					setStatusCodeResType(INTERNAL_ERROR, ERR_PAGE);
			}
			// std::cout << "cgi res size: " << fileSize(bodyFilePath) << std::endl;
			// std::cout << "cgi res size: " << cgiRes.size() << std::endl;
			// std::cout << "cgi res content: ";
			// printBytes(cgiRes);
			// std::cout << std::endl;
			cgiStage = FINISH_WAITING;
			if (op == KILL_PROCESS)
				std::cout<<"kill_process op"<<std::endl;
			std::cout<<"total byte extracted:"<<byteExtracted<<std::endl;
			std::cout<<"total byte written:"<<byteWritten<<std::endl;
			std::cout<<"total byte converted:"<<byteConverted<<std::endl;
		}
	}
	processCgiRes();
	if (isFinishWaitingStage() && !cgiRes.empty() && resultPage.empty() && !isChunkMode())
	{
		std::cout<<"convert cgi"<<std::endl;
		std::cout<<"cgi res size:"<<cgiRes.size()<<std::endl;
		if (!convertCGIResToResponse())
		{
			setStatusCodeResType(INTERNAL_ERROR, ERR_PAGE);
			std::cout<<"convert cgi res fail"<<std::endl;
		}
		std::cout << "Res page:" << std::endl;
		for (size_t i = 0; i < resultPage.size(); ++i)
			std::cout << resultPage[i];
		std::cout << std::endl;
		cgiRes.clear();
	}
	return 0;
}

ssize_t	Response::extractResultFromCgiPipe()
{
	ssize_t	readSize;

	if (cgiOutPipeDrained)
		return 0;
	readSize = read(cgiOutFd, buf, BUFFER_SIZE);
	if (readSize > 0)
	{
		updateCgiActiveTime();
		appendBuf(cgiRes, buf, readSize);
		byteExtracted += readSize;
		// std::cout << "read size: " << readSize << std::endl;
		// std::cout << "read from cgi out fd: " << cgiOutFd << std::endl;
	}
	if (readSize == -1)
		cgiOutPipeDrained = true;
	if (readSize == 0)
	{
		std::cout<<"pipe fd drained:"<<cgiOutFd<<std::endl;
		cgiOutPipeDrained = true;
	}
	return readSize;
}

ssize_t	Response::writeDataToCgiPipe()
{
	ssize_t		writeSize = 0;
	size_t		size = std::min((size_t)(BUFFER_SIZE / 2), request.getBody().size() - eraseLimit);

	if (size == 0 || isFinishWaitingStage())
		return 0;
	writeSize = write(cgiInFd, request.getBody().data() + eraseLimit, size);
	if (writeSize > 0)
	{
		updateCgiActiveTime();
		eraseLimit += writeSize;
		byteWritten += writeSize;
		if (eraseLimit >= 5000000)
		{
			request.removeNCharFromBody(eraseLimit);
			eraseLimit = 0;
		}
		
	}
	if (writeSize < 0)
	{
		if (!resultSent)
			setStatusCodeResType(INTERNAL_ERROR, ERR_PAGE);
		else
			cgiStage = FINISH_WAITING;
	}
	return writeSize;
}

void	Response::prepareArgEnv(std::string &exe, std::vector<std::string> &strs,
	std::vector<char *> &args, std::vector<char *> &env)
{
	args.push_back((char *)exe.c_str());
	args.push_back(NULL);
	for(std::vector<std::string>::const_iterator it = strs.begin(); it != strs.end(); ++it)
		env.push_back((char *)it->c_str());
	env.push_back(NULL);
}

void	Response::mergeEnvStrs(std::map<std::string, std::string> &allHeader, std::vector<std::string> &strs)
{
	std::string	value;

	for(std::map<std::string, std::string>::const_iterator it = allHeader.begin(); it != allHeader.end(); ++it)
	{
		value.reserve(it->first.size() + 1 + it->second.size());
		value = it->first;
		value.push_back('=');
		value.insert(value.end(), it->second.begin(), it->second.end());
		strs.push_back(value);
	}
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
	if (rType == ERR_PAGE)
	{
		resultPage.clear();
		if (isCGI())
		{
			cgiStage = FINISH_WAITING;
			cgiRes.clear();
		}
	}
	statusCode = code;
	resultType = rType;
}

void	Response::setCgiStage(int stage)
{
	cgiStage = stage;
}

void	Response::setResultSent(bool sent)
{
	resultSent = sent;
}

void	Response::determineResType(void)
{
	std::string	filePathStr;
	
	filePathStr = mergeFullPath(matchLocation->getRootFolder(),
		paths, locationMatchLength, matchLocation->getCGIConfig());
	logMessage(std::cout, "FilePath str: " + filePathStr);
	if (method == "DELETE")
	{
		resultType = DELETE_RESOURCE;
		resourcePath = filePathStr;
		return ;
	}
	if (matchLocation->isOneOfCGIConfig(filePathStr))
	{
		resultType = CGI_EXE;
		resourcePath = filePathStr;
	}
	else if (isDir(filePathStr))
	{
		if (*filePathStr.rbegin() != '/')
			filePathStr.push_back('/');
		logMessage(std::cout, "dir: " + filePathStr);
		resourcePath = matchLocation->findValidIndexPage(filePathStr);
		if (!resourcePath.empty() && matchLocation->isOneOfCGIConfig(resourcePath))
			resultType = CGI_EXE;
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
	if (isRegularFile(resourcePath) && fileExeOK(resourcePath))
	{
		resultType = CGI_EXE;
	}
	if (resultType == NONE)
		resultType = FILE;
}

void	Response::processCgiRes()
{
	Bytes::const_iterator	it;

	if (!cgiRes.empty() && isChunkMode())
	{
		if (!allHeaderExtracted)
			extractHeader(it);
		else
			appendBodyForChunkMode();
	}
	if (cgiRes.empty() && isChunkMode() && isCgiOutPipeDrained() && !endChunkAppended)
	{
		endChunkTransfer();
		endChunkAppended = true;
	}
}

void	Response::routeMatchingCheckLocationLimitationDetermineType()
{
	if (statusOK() && !matchLocation)
	{
		matchLocation = service->findMatchingRoute(paths);
		if (!matchLocation)
			(logMessage(std::cout, "no route match"), setStatusCodeResType(NOT_FOUND, ERR_PAGE));
	}
	if (statusOK() && matchLocation && resultType == NONE)
	{
		std::cout<<"route str:"<<matchLocation->getRouteStr()<<std::endl;
		locationMatchLength = matchLocation->getRouteMatchLength(paths);
		std::cout<<"match length:"<<locationMatchLength<<std::endl;
	}
	if (statusOK() && matchLocation && !matchLocation->isMethodAllowed(method))
		(logMessage(std::cout, "method not allowed"), setStatusCodeResType(NOT_ALLOWED, ERR_PAGE));
	checkBodySize();
	if (statusOK() && matchLocation && resultType == NONE)
		determineResType();
	if (statusOK() && !fileWriteOK(resourcePath) && resultType == DELETE_RESOURCE)
		(logMessage(std::cout, "file not writable"), setStatusCodeResType(FORBIDDEN, ERR_PAGE));
	if (statusOK() && matchLocation && !matchLocation->getRedirect().empty())
	{
		setStatusCodeResType(HTTP_REDIRECT, REDIRECT);
		headers.insert(std::make_pair("location", matchLocation->getRedirect()));
		method = "GET";
	}
	if (statusOK() && !fileExist(resourcePath) && resultType != CGI_EXE)
		(logMessage(std::cout, "file not found"), setStatusCodeResType(NOT_FOUND, ERR_PAGE));
	if (statusOK() && fileExist(resourcePath) && !fileReadOK(resourcePath))
		(logMessage(std::cout, "file not readable"), setStatusCodeResType(FORBIDDEN, ERR_PAGE));
}

void	Response::checkBodySize()
{
	if (statusOK() && matchLocation && (size_t)matchLocation->getMaxBodySize() < request.getBodyLength())
		(logMessage(std::cout, "body too large"), setStatusCodeResType(Content_Too_Large, ERR_PAGE));
}

void	Response::updateResultPage()
{
	std::string	res;

	if ((resultSent || !resultPage.empty()) && !isCGI() && (resultType != FILE || fileAllRead))
		return ;
	if (statusOK() && resultType == DELETE_RESOURCE)
		deleteResource();
	if (statusOK() && resultType == LIST_FOLDER)
		resultPage = matchLocation->generateIndexPages(resourcePath,
			mergeFullPath("", paths, locationMatchLength, false));
	if (statusOK() && resultType == CGI_EXE
		&& isRegularFile(resourcePath) && !fileExeOK(resourcePath))
		resultType = FILE;
	if (resultType == CGI_EXE)
		processCgi(PROCESS_DATA);
	if (statusOK() && resultType == FILE)
		getFileResponse();
	if ((resultType == REDIRECT || resultType == ERR_PAGE) && (!statusOK() || resultPage.empty()))
		stringToBytes(genHttpResponse(statusCode, headers, isHeadMethod(), res), resultPage);
	if (isStatusCodeinCustomErrorPage() && !prevStatusCode.count(statusCode))
	{
		std::string	newRoute = getCustomErrorPage().at(statusCode);
		std::vector<std::string> newPaths = splitPath(newRoute);

		if (!isExternalPath(newRoute) && !isRedirectStatusCode())
		{
			prevStatusCode.insert(statusCode);
			statusCode = HTTP_OK;
			init();
			paths = newPaths;
			method = "GET";
			route = newRoute;
			routeMatchingCheckLocationLimitationDetermineType();
		}
		else if (isExternalPath(newRoute))
		{
			setStatusCodeResType(HTTP_REDIRECT, REDIRECT);
			headers.erase("location");
			headers.insert(std::make_pair("location", newRoute));
			method = "GET";
			stringToBytes(genHttpResponse(statusCode, headers, isHeadMethod(), res), resultPage);
		}
	}
}

void	Response::deleteResource()
{
	std::string	res;

	if (!route.empty() &&
		((*route.rbegin() == '/' && isRegularFile(resourcePath))
		 || (*route.rbegin() != '/' && isDir(resourcePath))))
		setStatusCodeResType(CONFLICT, ERR_PAGE);
	else if (std::remove(resourcePath.c_str()))
		setStatusCodeResType(INTERNAL_ERROR, ERR_PAGE);
	else
	{
		setStatusCode(NO_CONTENT);
		stringToBytes(genHttpResponse(statusCode, headers, isHeadMethod(), res), resultPage);
	}
}

void	Response::clearResultPage()
{
	resultPage.clear();
}

void	Response::addHttpPrefixToHeaders(std::map<std::string, std::string> headersToAdd,
	std::map<std::string, std::string> &headersRes)
{
	for (std::map<std::string, std::string>::const_iterator it = headersToAdd.begin();
		it != headersToAdd.end(); ++it)
	{
		std::string	newKey = "HTTP_";

		newKey.insert(newKey.end(), it->first.begin(), it->first.end());
		std::transform(newKey.begin(), newKey.end(), newKey.begin(), transformHttpHeader);
		headersRes.insert(std::make_pair(newKey, it->second));
	}
}

void	Response::addCgiHeaders(std::map<std::string, std::string> &headersRes)
{
	//CGI env
	headersRes.insert(std::make_pair("SCRIPT_FILENAME", resourcePath));
	headersRes.insert(std::make_pair("PATH_INFO", "/"));
	headersRes.insert(std::make_pair("SERVER_PROTOCOL", request.getHttpVer()));
	headersRes.insert(std::make_pair("REQUEST_METHOD", method));
	headersRes.insert(std::make_pair("REDIRECT_STATUS", "200"));
	headersRes.insert(std::make_pair("SERVER_SOFTWARE", "webserv"));
	if (!request.isChunkMode())
		headersRes.insert(std::make_pair("CONTENT_LENGTH", toString(request.getBodyLength())));
	if (request.getHeaders().count(CONTENT_TYPE) > 0)
		headersRes.insert(std::make_pair("CONTENT_TYPE", request.getHeaders().at(CONTENT_TYPE)));
	else
		headersRes.insert(std::make_pair("CONTENT_TYPE", ""));
	headersRes.insert(std::make_pair("GATEWAY_INTERFACE", "CGI/1.1"));
}

void	Response::endChunkTransfer()
{
	std::string	zero = "0";

	resultPage.insert(resultPage.end(), zero.begin(), zero.end());
	resultPage.insert(resultPage.end(), CRLF.begin(), CRLF.end());
	resultPage.insert(resultPage.end(), CRLF.begin(), CRLF.end());
}

void	Response::removeNCharFromResultPage(size_t n)
{
	// std::cout << "deleting n char:" << n << std::endl;
	if (n <= resultPage.size())
		resultPage.erase(resultPage.begin(), resultPage.begin() + n);
	else
		resultPage.clear();
}

void	Response::updateCgiActiveTime()
{
	std::time(&cgiLastActiveTime);
}

void	Response::removeCgiResUpTo(Bytes::const_iterator it)
{
	size_t	size;

	size = std::distance(static_cast<Bytes::const_iterator>(cgiRes.begin()), it);
	cgiRes.erase(cgiRes.begin(), cgiRes.begin() + size);
}
