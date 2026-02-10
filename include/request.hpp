/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   request.hpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mayeung <mayeung@student.42london.com>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/13 18:45:46 by mayeung           #+#    #+#             */
/*   Updated: 2026/02/10 13:53:31 by mayeung          ###   ########.fr       */
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
		CHUNK_LENGTH,
		CHUNK_DATA,
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
		Bytes								&incomingData;
		int									statusCode;
		int									requestStatus;
		size_t								bodyLength;
		std::string							bodyFilePath;
		long long							expectedChunkSize;
		int									bodyFd;
		bool								updated;
		Request();
		std::string							parseReqLineSegment(const Bytes &delimiter);
		void								parseRequestLine();
		void								parseRequestHeader();
		void								splitRoute();
		void								parseBody();
		void								parseChunkLength();
		void								parseChunkData();
		void								extractContentLength(std::string &len);
		bool								readyparseChunkLength() const;
		bool								readyparseChunkData() const;
		bool								readyparseBody() const;
	public:
		static std::string	valMet[4];
		static std::vector<std::string>	validMethod;
		static std::string	valVer[1];
		static std::vector<std::string>	validHttpVersion;
		Request(Bytes::const_iterator newDataStart, Bytes::const_iterator newDataEnd, Bytes &inData);
		Request(const Request &right);
		~Request();
		Request										&operator=(const Request &right);
		void										parseRequest();
		bool										complete() const;
		bool										statusOK() const;
		bool										isMethod(std::string query) const;
		bool										isPostMethod() const;
		bool										isPutMethod() const;
		bool										isHeadMethod() const;
		bool										isWaitingChunk() const;
		bool										isChunkMode() const;
		bool										isUpdated() const;
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
		void										setIncomingData(Bytes &inData);
		void										setStatusCode(int code);
		void										setUpdated(bool b);
		void										clearBody();
		void										removeNCharFromBody(size_t n);
		void										setToComplete();
};
