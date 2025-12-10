/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   response.hpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mayeung <mayeung@student.42london.com>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/13 18:45:26 by mayeung           #+#    #+#             */
/*   Updated: 2025/12/10 17:56:33 by mayeung          ###   ########.fr       */
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
		std::ifstream	*pageStream;
		Bytes			resultPage;
		int				errorCode;
		Response();
		bool			tryOpenIndexPages();
	public:
		Response(Service &ser, Request &req);
		Response(const Response &right);
		~Response();
		Response			&operator=(const Response &right);
		const int			&getErrorCode() const;
		const Bytes			&getResultPage() const;
		const std::ifstream	*getPageStream() const;
		void				printResponse() const;
		Bytes				getPageStreamResponse(int code);
};
