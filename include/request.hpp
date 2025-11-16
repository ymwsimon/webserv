/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   request.hpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mayeung <mayeung@student.42london.com>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/13 18:45:46 by mayeung           #+#    #+#             */
/*   Updated: 2025/11/16 16:31:21 by mayeung          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once
#include <vector>
#include <string>
#include <map>
#include "utils.hpp"


class Request
{
	enum reqStatus
	{
		METHOD,
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
		Bytes								incomingData;
		int									errorCode;
		reqStatus							requestStatus;
	public:
		Request();
		~Request();
		void										parseMethod(const Bytes &newData);
		void										parseRoute(const Bytes &newData);
		void										parseHttpVersion(const Bytes &newData);
		void										parseRequestLine(const Bytes &newData);
		void										parseRequest(const Bytes &newData);
		bool										complete();
		void										appendData(Bytes &newData, size_t size);
		const std::string							&getMethod();
		const std::string							&getRoute();
		const std::string							&getHttpVer();
		const std::map<std::string, std::string>	&getHeaders();
		const Bytes									&getBody();
		const int									&getErrorCode();
		const reqStatus								&getReqStatus();
};
