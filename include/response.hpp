/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   response.hpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mayeung <mayeung@student.42london.com>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/13 18:45:26 by mayeung           #+#    #+#             */
/*   Updated: 2026/01/25 22:20:29 by mayeung          ###   ########.fr       */
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

const int	cgiWaitTime = 2;

enum e_cgiStage
{
	INIT,
	ADD_FD_POLL,
	WAITING_CGI,
	FINISH_WAITING,
};

enum e_cgiEvent
{
	EXTRACT_PIPE,
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
		DELETE_RESOURCE,
		ERR_PAGE,
	};
	private:
		Service								*service;
		Request								&request;
		const Location						*matchLocation;
		std::string							resourcePath;
		std::ifstream						*pageStream;
		int									cgiResFd;
		int									cgiStage;
		Bytes								resultPage;
		Bytes								cgiRes;
		int									statusCode;
		int									resultType;
		pid_t								cgiPid;
		int									pipeFd[2];
		std::time_t							cgiStartTime;
		std::map<std::string, std::string>	headers;
		size_t								locationMatchLength;
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
		int					getStatusCode() const;
		const Bytes			&getResultPage() const;
		const Location		*getMatchLocation() const;
		const std::string	getResourcePath() const;
		const std::ifstream	*getPageStream() const;
		int					getResultType() const;
		int					getCgiResFd() const;
		int					getCgiStage() const;
		void				printResponse() const;
		void				getFileResponse();
		bool				convertCGIResToResponse();
		void				extractHeader(const Bytes &cgiRes, std::map<std::string, std::string> &headers, Bytes::const_iterator &crlfPos);
		void				startCgi();
		void				processCgi(int op);
		void				prepareArgEnv(std::string exe, std::vector<std::string> &strs, std::vector<char *> &args, std::vector<char *> &env);
		void				setStatusCode(int code);
		void				setMatchLocation(const Location *location);
		void				setResourcePath(const std::string path);
		void				setStatusCodeResType(int code, int rType);
		void				setCgiStage(int stage);
		void				processResponse();
		void				deleteResource();
};
