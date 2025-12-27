/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   response.hpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mayeung <mayeung@student.42london.com>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/13 18:45:26 by mayeung           #+#    #+#             */
/*   Updated: 2025/12/17 16:24:56 by mayeung          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once
#include <vector>
#include <fstream>
#include <sys/stat.h>
#include "service.hpp"
#include "request.hpp"
#include "utils.hpp"

class Response
{
	private:
		Service			&service;
		Request			&request;
		const Location	*matchLocation;
		std::ifstream	*pageStream;
		Bytes			resultPage;
		int				statusCode;
		Response();
		bool			tryOpenIndexPages();
	public:
		Response(Service &ser, Request &req);
		Response(const Response &right);
		~Response();
		Response			&operator=(const Response &right);
		bool				statusOK() const;
		int					getStatusCode() const;
		const Bytes			&getResultPage() const;
		const Location		*getMatchLocation() const;
		const std::ifstream	*getPageStream() const;
		void				printResponse() const;
		Bytes				getPageStreamResponse(int code);
		void				setStatusCode(int code);
		void				setMatchLocation(const Location *);
};
