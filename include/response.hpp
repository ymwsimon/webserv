/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   response.hpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mayeung <mayeung@student.42london.com>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/13 18:45:26 by mayeung           #+#    #+#             */
/*   Updated: 2026/03/05 16:55:41 by mayeung          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once
#include <vector>
#include <set>
#include <fstream>
#include <string>
#include <ctime>
#include <cstdio>
#include <signal.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <unistd.h>
#include "service.hpp"
#include "request.hpp"
#include "utils.hpp"

const int	cgiWaitTime = 3;
extern int	g_error;
enum e_cgiStage
{
	INIT,
	ADD_FD_POLL,
	WAITING_HEADER,
	WAITING_CGI_BODY,
	FINISH_WAITING,
};

enum e_cgiEvent
{
	EXTRACT_PIPE,
	WRITE_PIPE,
	KILL_PROCESS,
	PROCESS_DATA,
};

class Response
{
	enum e_resType
	{
		NONE,
		CGI_EXE,
		FILE,
		LIST_FOLDER,
		REDIRECT,
		DELETE_RESOURCE,
		ERR_PAGE,
	};
	private:
		Service								*service;
		Request								&request;
		std::string							route;
		std::vector<std::string> 			paths;
		std::string							method;
		const Location						*matchLocation;
		std::set<int>						prevStatusCode;
		std::string							resourcePath;
		int									cgiOutFd;
		int									cgiInFd;
		size_t								byteWritten;
		size_t								byteExtracted;
		size_t								byteConverted;
		size_t								eraseLimit;
		int									cgiStage;
		Bytes								resultPage;
		Bytes								cgiRes;
		int									statusCode;
		int									resultType;
		pid_t								cgiPid;
		pid_t								waitRes;
		int									waitStatus;
		int									outPipeFd[2];
		int									inPipeFd[2];
		std::time_t							cgiLastActiveTime;
		std::map<std::string, std::string>	headers;
		size_t								locationMatchLength;
		std::string							bodyFilePath;
		std::string							cgiExeBasePath;
		Byte								buf[BUFFER_SIZE];
		bool								resultSent;
		bool								cgiOutPipeDrained;
		bool								allHeaderExtracted;
		bool								endChunkAppended;
		bool								fileAllRead;
		int									fileFd;
		Response();
		void								init();
		void								determineResType();
		void								processCgiRes();
	public:
		Response(Service *ser, Request &req);
		Response(const Response &right);
		~Response();
		Response			&operator=(const Response &right);
		bool				statusOK() const;
		bool				isNoneType() const;
		bool				isFileType() const;
		bool				isCGI() const;
		bool				isINITStage() const;
		bool				isAddFdStage() const;
		bool				isWaitingStage() const;
		bool				isFinishWaitingStage() const;
		bool				needCloseCgiInFd() const;
		bool				isChunkMode() const;
		bool				gotEnoughChunkDataToSent() const;
		bool				isResultPageEmpty() const;
		bool				isResultSent() const;
		bool				isCgiOutPipeDrained() const;
		bool				isEndChunkAppended() const;
		bool				isHeadMethod() const;
		bool				isStatusCodeinCustomErrorPage() const;
		bool				isRedirectStatusCode() const;
		bool				isFileAllRead() const;
		int					getStatusCode() const;
		const Bytes			&getResultPage() const;
		const Bytes			&getCgiRes() const;
		const Location		*getMatchLocation() const;
		const std::string	getResourcePath() const;
		int					getResultType() const;
		int					getCgiOutFd() const;
		int					getCgiInFd() const;
		int					getCgiStage() const;
		const std::map<int, std::string>	&getCustomErrorPage() const;
		void				printResponse() const;
		void				getFileResponse();
		std::string			getBodyFilePath() const;
		size_t				getByteWritten() const;
		size_t				getEraseLimit() const;
		pid_t				getPid() const;
		pid_t				getWaitRes() const;
		int					getWaitStatus() const;
		
		bool				convertCGIResToResponse();
		void				appendHeaderToResultPage(Bytes::const_iterator	crlfPos);
		void				extractHeader(Bytes::const_iterator &crlfPos);
		void				appendBodyForChunkMode();
		void				startCgi();
		ssize_t				processCgi(int op);
		ssize_t				extractResultFromCgiPipe();
		ssize_t				writeDataToCgiPipe();
		void				prepareArgEnv(std::string &exe, std::vector<std::string> &strs, std::vector<char *> &args, std::vector<char *> &env);
		void				mergeEnvStrs(std::map<std::string, std::string> &allHeader, std::vector<std::string> &strs);
		void				setStatusCode(int code);
		void				setMatchLocation(const Location *location);
		void				setResourcePath(const std::string path);
		void				setStatusCodeResType(int code, int rType);
		void				setCgiStage(int stage);
		void				setResultSent(bool sent);
		void				routeMatchingCheckLocationLimitationDetermineType();
		void				checkBodySize();
		void				updateResultPage();
		void				deleteResource();
		void				clearResultPage();
		void				addHttpPrefixToHeaders(std::map<std::string, std::string> headersToAdd,
			std::map<std::string, std::string> &headersRes);
		void				addCgiHeaders(std::map<std::string, std::string> &headersRes);
		void				endChunkTransfer();
		void				removeNCharFromResultPage(size_t n);
		void				updateCgiActiveTime();
		void				removeCgiResUpTo(Bytes::const_iterator it);
};
