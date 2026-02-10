/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   response.hpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mayeung <mayeung@student.42london.com>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/13 18:45:26 by mayeung           #+#    #+#             */
/*   Updated: 2026/02/10 15:36:22 by mayeung          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once
#include <vector>
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

const int	cgiWaitTime = 10;

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
		const Location						*matchLocation;
		std::string							resourcePath;
		std::ifstream						*pageStream;
		int									cgiOutFd;
		int									cgiInFd;
		size_t								byteWritten;
		int									cgiStage;
		Bytes								resultPage;
		Bytes								cgiRes;
		int									statusCode;
		int									resultType;
		pid_t								cgiPid;
		int									outPipeFd[2];
		int									inPipeFd[2];
		std::time_t							cgiLastActiveTime;
		std::map<std::string, std::string>	headers;
		size_t								locationMatchLength;
		std::string							bodyFilePath;
		Response();
		void			determineResType();
	public:
		Response(Service *ser, Request &req);
		Response(const Response &right);
		~Response();
		Response			&operator=(const Response &right);
		bool				statusOK() const;
		bool				isNoneType() const;
		bool				isCGI() const;
		bool				isINITStage() const;
		bool				isAddFdStage() const;
		bool				isWaitingStage() const;
		bool				isFinishWaitingStage() const;
		bool				needCloseCgiInFd() const;
		bool				isChunkMode() const;
		bool				gotEnoughChunkDataToSent() const;
		int					getStatusCode() const;
		const Bytes			&getResultPage() const;
		const Location		*getMatchLocation() const;
		const std::string	getResourcePath() const;
		const std::ifstream	*getPageStream() const;
		int					getResultType() const;
		int					getCgiOutFd() const;
		int					getCgiInFd() const;
		int					getCgiStage() const;
		void				printResponse() const;
		void				getFileResponse();
		std::string			getBodyFilePath() const;
		size_t				getByteWritten() const;
		bool				convertCGIResToResponse();
		void				appendHeaderToResultPage(Bytes::const_iterator	crlfPos);
		void				extracCgitHeader();
		void				extractHeader(Bytes::const_iterator &crlfPos);
		void				appendBodyForChunkMode();
		void				startCgi();
		void				processCgi(int op);
		void				prepareArgEnv(std::string exe, std::vector<std::string> &strs, std::vector<char *> &args, std::vector<char *> &env);
		void				mergeEnvStrs(std::map<std::string, std::string> &allHeader, std::vector<std::string> &strs);
		void				setStatusCode(int code);
		void				setMatchLocation(const Location *location);
		void				setResourcePath(const std::string path);
		void				setStatusCodeResType(int code, int rType);
		void				setCgiStage(int stage);
		void				processResponse();
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
