/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   request.hpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mayeung <mayeung@student.42london.com>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/13 18:45:46 by mayeung           #+#    #+#             */
/*   Updated: 2025/11/16 23:24:28 by mayeung          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once
#include <vector>
#include <string>
#include <map>
#include <utility>
#include <iostream>
#include "utils.hpp"

class Request
{
	static inline std::vector<std::string>	validMethod = {"POST", "GET", "DELETE"};
	static inline std::vector<std::string>	validHttpVersion = {"HTTP/1.1"};

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
		std::string							httpVer;
		std::map<std::string, std::string>	headers;
		Bytes								body;
		Bytes::const_iterator 				newDataStart;
		Bytes::const_iterator 				newDataEnd;
		int									errorCode;
		int									requestStatus;
		Request();
	public:
		Request(Bytes::const_iterator newDataStart, Bytes::const_iterator newDataEnd);
		~Request();
		std::string									parseReqLineSegment(const Bytes &delimiter);
		void										parseRequestHeader();
		void										parseBody();
		void										parseRequestLine();
		void										parseRequest();
		bool										complete();
		void										printRequest();
		const std::string							&getMethod();
		const std::string							&getRoute();
		const std::string							&getHttpVer();
		const std::map<std::string, std::string>	&getHeaders();
		const Bytes									&getBody();
		const int									&getErrorCode();
		const reqStatus								&getReqStatus();
		Bytes::const_iterator						&getDataStart();
		Bytes::const_iterator						&getDataEnd();
		void										setDataStart(Bytes::const_iterator &s);
		void										setDataEnd(Bytes::const_iterator &e);
};
