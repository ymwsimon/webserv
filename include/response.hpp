/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   response.hpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mayeung <mayeung@student.42london.com>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/13 18:45:26 by mayeung           #+#    #+#             */
/*   Updated: 2026/01/07 21:21:20 by mayeung          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once
#include <vector>
#include <fstream>
#include <sys/stat.h>
#include <sys/wait.h>
#include <unistd.h>
#include "service.hpp"
#include "request.hpp"
#include "utils.hpp"

class Response
{
	private:
		Service			&service;
		Request			&request;
		const Location	*matchLocation;
		std::string		resourcePath;
		std::ifstream	*pageStream;
		Bytes			resultPage;
		int				statusCode;
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
		Bytes				exeCGI(std::string exe);
		void				setStatusCode(int code);
		void				setMatchLocation(const Location *location);
		void				setResourcePath(const std::string path);
};
