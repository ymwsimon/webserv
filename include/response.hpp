/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   response.hpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mayeung <mayeung@student.42london.com>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/13 18:45:26 by mayeung           #+#    #+#             */
/*   Updated: 2025/11/25 23:07:07 by mayeung          ###   ########.fr       */
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
		Response(const Response &other);
		Response(Service &ser, Request &req);
		~Response();
		Response	&operator=(const Response &right);
		const int	&getErrorCode() const;
		const Bytes	&getResultPage() const;
		const std::ifstream	*getPageStream();
		void		printResponse() const;
		Bytes		getOKResponse();
};
