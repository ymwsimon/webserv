/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   response.cpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mayeung <mayeung@student.42london.com>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/17 14:05:04 by mayeung           #+#    #+#             */
/*   Updated: 2026/02/01 18:25:06 by mayeung          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../include/response.hpp"

Response::Response(Service *ser, Request &req) : service(ser), request(req), statusCode(req.getStatusCode())
{
	resultType = NONE;
	cgiStage = INIT;
	if (!statusOK())
		resultType = ERR_PAGE;
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
		headers = right.headers;
		locationMatchLength = right.locationMatchLength;
		bodyFilePath = right.bodyFilePath;
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

void	Response::getFileResponse()
{
	Bytes				buf(BUFFER_SIZE);
	std::string			head;
	int					fd;
	int					readSize;

	fd = open(resourcePath.c_str(), O_RDONLY, 0700);
	while (fd >= 0 && (readSize = read(fd, buf.data(), BUFFER_SIZE)) > 0)
		appendBuf(resultPage, buf, readSize);
	if (fd < 0
		|| close(fd) < 0
		|| readSize < 0)
	{
		setStatusCodeResType(INTERNAL_ERROR, ERR_PAGE);
		return ;
	}
	head = genHttpResponseLine(statusCode);
	head += genHttpHeader(CONTENT_TYPE, getMediaType(extractFileExt(resourcePath)));
	head += genHttpHeader(CONTENT_LENGTH, toString(resultPage.size()));
	head += CRLFStr;
	resultPage.insert(resultPage.begin(), head.begin(), head.end());
}

std::string	Response::getBodyFilePath() const
{
	return bodyFilePath;
}

bool	Response::convertCGIResToResponse()
{
	Bytes::const_iterator				crlfPos;
	int									size;
	int									fd;
	Bytes								buf(BUFFER_SIZE);

	fd = open(bodyFilePath.c_str(), O_RDONLY, 0700);
	while ((size = read(fd, buf.data(), BUFFER_SIZE)) > 0)
		appendBuf(cgiRes, buf, size);
	extractHeader(cgiRes, crlfPos);
	if (headers.empty() || headers.count(CONTENT_TYPE) == 0)
	{
		std::cout<<"empty header"<<std::endl;
		setStatusCodeResType(INTERNAL_ERROR, ERR_PAGE);
		return false;
	}
	if (headers.count("status") > 0 && headers.at("status") != "200 OK")
	{
		std::cout<<"status err"<<std::endl;
		std::cout<<"status: "<<headers.at("status") <<std::endl;

		// statusCode = toInt(headers.at("status"));
		setStatusCodeResType(INTERNAL_ERROR, ERR_PAGE);
		// if (statusCode == -1)
		// 	statusCode = INTERNAL_ERROR;
		return false;
	}
	appendBytes(resultPage, genHttpResponseLine(200));
	for (std::map<std::string, std::string>::iterator headerIt = headers.begin();
		headerIt != headers.end(); ++headerIt)
		appendBytes(resultPage, genHttpHeader(headerIt->first, headerIt->second));
	size = std::distance(crlfPos + CRLF.size(), static_cast<Bytes::const_iterator> (cgiRes.end()));
	appendBytes(resultPage, genHttpHeader(CONTENT_LENGTH, toString(size)));
	appendBytes(resultPage, CRLFStr);
	appendBytes(resultPage, crlfPos + CRLF.size(), cgiRes.end());
	return true;
}

void	Response::extractHeader(const Bytes &cgiRes, Bytes::const_iterator &crlfPos)
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
	}
}

void	Response::startCgi()
{
	std::vector<std::string>	strs(10);
	std::vector<char *>			args(2);
	std::vector<char *>			env(11);
	int							fd;
	std::string					exe;

	exe = matchLocation->findCGIExecutable(extractFileExt(resourcePath));
	std::cout<<"resource path:"<<resourcePath<<std::endl;
	std::cout<<"exe path:"<<exe<<std::endl;
	std::srand(std::time(NULL));
	bodyFilePath = "tmp/" + toString(std::rand());
	while (fileExist(bodyFilePath))
		bodyFilePath = "tmp/" + toString(std::rand());
	fd = open(bodyFilePath.c_str(), O_CREAT | O_TRUNC, 0700);
	if (fd < 0)
	{
		setStatusCodeResType(INTERNAL_ERROR, ERR_PAGE);
		return ;
	}
	if (exe.empty() && fileExeOK(resourcePath))
		exe = resourcePath;
	// if (pipe(pipeFd) < 0)
	// {
	// 	std::cout << "create pipe fail" << std::endl;
	// 	setStatusCodeResType(INTERNAL_ERROR, ERR_PAGE);
	// 	cgiStage = FINISH_WAITING;
	// 	return ;
	// }
	// cgiResFd = pipeFd[0];
	cgiPid = fork();
	if (cgiPid < 0)
	{
		// close(pipeFd[0]);
		// close(pipeFd[1]);
		setStatusCodeResType(INTERNAL_ERROR, ERR_PAGE);
		cgiStage = FINISH_WAITING;
		return ;
	}
	if (!cgiPid)
	{
		std::string	inBodyPath = request.getBodyFilePath();

		prepareArgEnv(exe, strs, args, env);
		// if (close(pipeFd[0]) < 0)
		// 	std::exit(1);
		// if (bodyPath.empty())
		// {
		// 	bodyPath = toString(pipeFd[0]);
		// 	fd = open(bodyPath.c_str(), O_WRONLY | O_CREAT | O_TRUNC, 0700);
		// 	if (fd < 0
		// 		|| write(fd, request.getBody().data(), request.getBody().size()) < 0
		// 		|| close(fd) < 0)
		// 		std::exit(1);
		// }
		// std::cerr<<"bodyPath:"<<bodyPath<<std::endl;
		fd = open(inBodyPath.c_str(), O_RDONLY, 0700);
		if (fd < 0
			|| dup2(fd, STDIN_FILENO) < 0
			|| close(fd) < 0)
			std::exit(1);
		fd = open(bodyFilePath.c_str(), O_WRONLY, 0700);
		if (fd < 0
			|| dup2(fd, STDOUT_FILENO) < 0
			|| close(fd) < 0)
			std::exit(1);
		execve(exe.c_str(), (char *const*)args.data(), (char *const*)env.data());
		std::exit(1);
	}
	else
	{
		// if (close(pipeFd[1]) < 0)
		// {
		// 	setStatusCodeResType(INTERNAL_ERROR, ERR_PAGE);
		// 	cgiStage = FINISH_WAITING;
		// 	return ;
		// }
		std::time(&cgiStartTime);
		// cgiResFd = pipeFd[0];
		// cgiStage = ADD_FD_POLL;
		cgiStage = WAITING_CGI;
	}
}

void	Response::processCgi(int op)
{
	int				status;
	pid_t			waitRes;
	// unsigned char	buf[BUFFER_SIZE];
	int				readSize = 0;

	// std::cerr<<"process cgi"<<std::endl;
	if (cgiStage == INIT)
		startCgi();
	else if (cgiStage == WAITING_CGI || cgiStage == ADD_FD_POLL)
	{
		if (cgiStage == ADD_FD_POLL)
			cgiStage = WAITING_CGI;
		// if (op == EXTRACT_PIPE && (readSize = read(cgiResFd, buf, BUFFER_SIZE)) > 0)
		// {
		// 	// std::cout << "extracting data from cgi pipe" << std::endl;
		// 	appendBuf(cgiRes, buf, readSize);
		// 	// std::cout << "read size: " << readSize << std::endl;
		// 	cgiStartTime = std::time(NULL);
		// }
		if (readSize < 0
			|| (waitRes = waitpid(cgiPid, &status, WUNTRACED | WNOHANG)) < 0)
			// || (WIFEXITED(status) && WEXITSTATUS(status) != 0))
		{
			setStatusCodeResType(INTERNAL_ERROR, ERR_PAGE);
			return ;
		}
		if ((!waitRes && std::difftime(std::time(NULL), cgiStartTime) > cgiWaitTime)
			|| op == KILL_PROCESS)
		{
			std::cout << "time to kill cgiPid: " << cgiPid << std::endl;
			// kill(cgiPid, SIGKILL);
			// setStatusCodeResType(INTERNAL_ERROR, ERR_PAGE);
			std::cout << "finish kill cgiPid: " << cgiPid << std::endl;
		}
		if (waitRes == cgiPid)
			// || (!waitRes && difftime(std::time(NULL), cgiStartTime) > cgiWaitTime)
			// || op == KILL_PROCESS)
		{
			waitpid(cgiPid, &status, WUNTRACED | WNOHANG);
			if (WIFEXITED(status) && WEXITSTATUS(status) != 0)
				setStatusCodeResType(INTERNAL_ERROR, ERR_PAGE);
			std::cout<< "bodyfilepath:"<<bodyFilePath<<std::endl;
			std::cout << "cgi res size: " << fileSize(bodyFilePath) << std::endl;
			// std::cout << "cgi res size: " << cgiRes.size() << std::endl;
			// std::cout << "cgi res content: ";
			// printBytes(cgiRes);
			// std::cout << std::endl;
			cgiStage = FINISH_WAITING;
		}
	}
	// std::cout<<"finishwait"<<std::endl;
	if (isFinishWaitingStage() && resultPage.empty())
	{
		// setStatusCodeResType(INTERNAL_ERROR, ERR_PAGE);
		std::cout<<"convert cgi"<<std::endl;
		if (!convertCGIResToResponse())
		{
			setStatusCodeResType(INTERNAL_ERROR, ERR_PAGE);
			std::cout<<"convert cgi res fail"<<std::endl;
		}
	}
	// std::cerr<<"process cgi fin"<<std::endl;
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
	strs[8] = "HTTP_TRANSFER_ENCODING=chunked";
	strs[9] = "HTTP_X_SECRET_HEADER_FOR_TEST=1";
	strs[0] += resourcePath;
	strs[1] += "/";
	strs[2] += request.getHttpVer();
	strs[3] += request.getMethod();
	strs[4] += "200";
	if (!request.getBodyFilePath().empty())
		strs[5] += toString(fileSize(request.getBodyFilePath()));
	else
		strs[5] += toString(request.getBodyLength());
	if (request.getHeaders().count(CONTENT_TYPE) > 0)
		strs[6] += request.getHeaders().at(CONTENT_TYPE);
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
	env[8] = (char *) strs[8].c_str();
	env[9] = (char *) strs[9].c_str();
	env[10] = NULL;
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

void	Response::determineResType(void)
{
	std::string	filePathStr;
	
	filePathStr = mergeFullPath(matchLocation->getRootFolder(),
		request.getPaths(), locationMatchLength, matchLocation->getCGIConfig());
	logMessage(std::cout, "FilePath str: " + filePathStr);
	if (request.isMethod("DELETE"))
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
	// std::cout <<"filepathstr:"<<filePathStr<<std::endl;
	// if (fileWithExt(filePathStr, "bla"))
	// {
	// 	std::cout <<"end with bla"<<std::endl;
	// 	resultType = CGI_EXE;
	// 	if (filePathStr == "./data/www/YoupiBanane/youpla.bla")
	// 		resourcePath = "./data/www/YoupiBanane/youpi.bla";
	// 	// if (!fileExist(filePathStr))
	// 	// {
	// 	// 	setStatusCodeResType(200, ERR_PAGE);
	// 	// 	headers.insert(std::make_pair("Status", getFullStatusMessage(200)));
	// 	// }
	// }
	if (resultType == NONE)
		resultType = FILE;
}

void	Response::processResponse()
{
	if (!resultPage.empty())
		return ;
	if (!statusOK() || resultType == ERR_PAGE)
		resultPage = stringToBytes(genHttpResponse(statusCode, headers));
	if (statusOK())
		matchLocation = service->findMatchingRoute(request);
	if (statusOK() && !matchLocation)
		(logMessage(std::cout, "no route match"), setStatusCodeResType(NOT_FOUND, ERR_PAGE));
	if (statusOK() && matchLocation && resultType == NONE)
	{
		std::cout<<"route str:"<<matchLocation->getRouteStr()<<std::endl;
		locationMatchLength = matchLocation->getRouteMatchLength(request.getPaths());
		std::cout<<"match length:"<<locationMatchLength<<std::endl;
	}
	if (statusOK() && request.getMethod() == "HEAD")
		(logMessage(std::cout, "head not allowed"), setStatusCodeResType(NOT_ALLOWED, ERR_PAGE));
	if (statusOK() && matchLocation && !matchLocation->isMethodAllowed(request.getMethod()))
		(logMessage(std::cout, "method not allowed"), setStatusCodeResType(NOT_ALLOWED, ERR_PAGE));
	if (statusOK() && matchLocation && (size_t)matchLocation->getMaxBodySize() < request.getBodyLength())
		(logMessage(std::cout, "body too large"), setStatusCodeResType(Content_Too_Large, ERR_PAGE));
	if (statusOK() && matchLocation && resultType == NONE)
		determineResType();
	if (statusOK() && !fileExist(resourcePath) && resultType != CGI_EXE)
		(logMessage(std::cout, "file not found"), setStatusCodeResType(NOT_FOUND, ERR_PAGE));
	if (statusOK() && fileExist(resourcePath) && !fileReadOK(resourcePath))
		(logMessage(std::cout, "file not readable"), setStatusCodeResType(FORBIDDEN, ERR_PAGE));
	if (statusOK() && resultType == DELETE_RESOURCE)
		deleteResource();
	if (statusOK() && resultType == LIST_FOLDER)
		resultPage = matchLocation->generateIndexPages(resourcePath,
			mergeFullPath("", request.getPaths(), locationMatchLength, false));
	if (statusOK() && resultType == CGI_EXE
		&& isRegularFile(resourcePath) && !fileExeOK(resourcePath))
		resultType = FILE;
	if (statusOK() && resultType == CGI_EXE)
		processCgi(PROCESS_DATA);
	if (statusOK() && resultType == FILE)
		getFileResponse();
}

void	Response::deleteResource()
{
	if (!request.getRoute().empty() &&
		((*request.getRoute().rbegin() == '/' && isRegularFile(resourcePath))
		 || (*request.getRoute().rbegin() != '/' && isDir(resourcePath))))
		setStatusCodeResType(CONFLICT, ERR_PAGE);
	else if (std::remove(resourcePath.c_str()))
		setStatusCodeResType(INTERNAL_ERROR, ERR_PAGE);
	else
	{
		setStatusCode(NO_CONTENT);
		resultPage = stringToBytes(genHttpResponse(statusCode));
	}
}

void	Response::clearResultPage()
{
	resultPage.clear();
}
