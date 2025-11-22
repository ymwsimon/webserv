/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   request.hpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mayeung <mayeung@student.42london.com>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/13 18:45:46 by mayeung           #+#    #+#             */
/*   Updated: 2025/11/22 00:48:59 by mayeung          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once
#include <vector>
#include <string>
#include <sstream>
#include <map>
#include <utility>
#include <iostream>
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
		COMPLETE
	};

	private:
		std::string							method;
		std::string							route;
		std::vector<std::string>			paths;
		std::string							fileName;
		std::string							httpVer;
		std::map<std::string, std::string>	headers;
		Bytes								body;
		Bytes::const_iterator 				newDataStart;
		Bytes::const_iterator 				newDataEnd;
		int									errorCode;
		int									requestStatus;
		size_t								bodyLength;
		const Location						*matchLocation;
		Request();
		std::string							parseReqLineSegment(const Bytes &delimiter);
		void								parseRequestLine();
		void								parseRequestHeader();
		void								splitRoute();
		void								parseBody();
		void								extractContentLength(std::string &len);
	public:
		static std::string	valMet[3];
		static std::vector<std::string>	validMethod;
		static std::string	valVer[1];
		static std::vector<std::string>	validHttpVersion;
		Request(Bytes::const_iterator newDataStart, Bytes::const_iterator newDataEnd);
		~Request();
		void										parseRequest();
		bool										complete();
		void										printRequest();
		const std::string							&getMethod() const;
		const std::string							&getRoute() const;
		const std::string							&getHttpVer() const;
		const std::map<std::string, std::string>	&getHeaders() const;
		const Bytes									&getBody() const;
		const int									&getErrorCode() const;
		const reqStatus								&getReqStatus() const;
		const size_t								&getBodyLength() const;
		const std::vector<std::string>				&getPaths() const;
		const std::string							&getFileName() const;
		Bytes::const_iterator						&getDataStart();
		Bytes::const_iterator						&getDataEnd();
		const Location								*getMatchLocation() const;
		void										setDataStart(Bytes::const_iterator s);
		void										setDataEnd(Bytes::const_iterator e);
		void										setMatchLocation(const Location *);
		Request										&operator=(const Request &right);
};
