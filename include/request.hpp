/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   request.hpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mayeung <mayeung@student.42london.com>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/13 18:45:46 by mayeung           #+#    #+#             */
/*   Updated: 2026/01/31 20:34:03 by mayeung          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once
#include <vector>
#include <string>
#include <sstream>
#include <map>
#include <utility>
#include <iostream>
#include <cstdlib>
#include <ctime>
#include <cstdlib>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include "location.hpp"
#include "utils.hpp"

class Request
{
	enum reqStatus
	{
		METHOD = 0,
		ROUTE,
		HTTPVERSION,
		HEADERS,
		BODY,
		COMPLETE,
		WAITING_CHUNK,
	};

	private:
		std::string							method;
		std::string							route;
		std::vector<std::string>			paths;
		std::string							httpVer;
		std::map<std::string, std::string>	headers;
		Bytes								body;
		Bytes::const_iterator 				newDataStart;
		Bytes::const_iterator 				newDataEnd;
		int									statusCode;
		int									requestStatus;
		size_t								bodyLength;
		std::string							bodyFilePath;
		long long							expectedChunkSize;
		int									bodyFd;
		Request();
		std::string							parseReqLineSegment(const Bytes &delimiter);
		void								parseRequestLine();
		void								parseRequestHeader();
		void								splitRoute();
		void								parseBody();
		void								parseChunk();
		void								extractContentLength(std::string &len);
		bool								isPostMethod() const;
		bool								isPutMethod() const;
		bool								switchToChunkMode();
	public:
		static std::string	valMet[4];
		static std::vector<std::string>	validMethod;
		static std::string	valVer[1];
		static std::vector<std::string>	validHttpVersion;
		Request(Bytes::const_iterator newDataStart, Bytes::const_iterator newDataEnd);
		Request(const Request &right);
		~Request();
		Request										&operator=(const Request &right);
		void										parseRequest();
		bool										complete() const;
		bool										statusOK() const;
		bool										isMethod(std::string query) const;
		bool										isWaitingChunk() const;
		void										printRequest() const;
		const std::string							&getMethod() const;
		const std::string							&getRoute() const;
		const std::string							&getHttpVer() const;
		const std::map<std::string, std::string>	&getHeaders() const;
		const Bytes									&getBody() const;
		int											getStatusCode() const;
		const reqStatus								&getReqStatus() const;
		size_t										getBodyLength() const;
		std::string									getBodyFilePath() const;
		const std::vector<std::string>				&getPaths() const;
		Bytes::const_iterator						getDataStart() const;
		Bytes::const_iterator						getDataEnd() const;
		void										setDataStart(Bytes::const_iterator s);
		void										setDataEnd(Bytes::const_iterator e);
		void										setStatusCode(int code);
};
