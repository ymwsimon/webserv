/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   response.hpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mayeung <mayeung@student.42london.com>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/13 18:45:26 by mayeung           #+#    #+#             */
/*   Updated: 2026/01/12 23:14:03 by mayeung          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once
#include <vector>
#include <fstream>
#include <string>
#include <ctime>
#include <signal.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <unistd.h>
#include "service.hpp"
#include "request.hpp"
#include "utils.hpp"

const int	cgiWaitTime = 2;

class Response
{
	enum e_resType
	{
		NONE,
		CGI_EXE,
		FILE,
		LIST_FOLDER,
		ERR_PAGE,
	};
	private:
		Service			&service;
		Request			&request;
		const Location	*matchLocation;
		std::string		resourcePath;
		std::ifstream	*pageStream;
		Bytes			resultPage;
		int				statusCode;
		int				resultType;
		Response();
	public:
		Response(Service &ser, Request &req);
		Response(const Response &right);
		~Response();
		Response			&operator=(const Response &right);
		bool				statusOK() const;
		int					getStatusCode() const;
		const Bytes			&getResultPage() const;
		const Location		*getMatchLocation() const;
		const std::string	getResourcePath() const;
		const std::ifstream	*getPageStream() const;
		void				printResponse() const;
		Bytes				getPageStreamResponse();
		bool				openPageStream();
		Bytes				convertCGIResToResponse(const Bytes &cgiRes);
		void				extractHeader(const Bytes &cgiRes, std::map<std::string, std::string> &headers, Bytes::const_iterator &crlfPos);
		void				exeCGI(std::string exe, Bytes &res);
		bool				cgiParent(pid_t pid, int *pipeFd);
		void				cgiExtractResult(Bytes &res, int *pipeFd);
		void				prepareArgEnv(std::string exe, std::vector<std::string> &strs, std::vector<char *> &args, std::vector<char *> &env);
		void				setStatusCode(int code);
		void				setMatchLocation(const Location *location);
		void				setResourcePath(const std::string path);
		void				setStatusCodeResType(int code, int rType);
		void				handleCGIExe(void);
		void				determineResType(void);
};
