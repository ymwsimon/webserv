/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   response.hpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mayeung <mayeung@student.42london.com>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/13 18:45:26 by mayeung           #+#    #+#             */
/*   Updated: 2025/11/22 02:05:26 by mayeung          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once
#include <vector>
#include <fstream>
#include "service.hpp"
#include "request.hpp"
#include "utils.hpp"

class Response
{
	private:
		Service			&service;
		Request			&request;
		std::ifstream	*pageStream;
		int				errorCode;
		Response();
	public:
		Response(const Response &other);
		Response(Service &ser, Request &req);
		~Response();
		Response	&operator=(const Response &right);
		const int	&getErrorCode() const;
		void		printResponse() const;
		Bytes		getOKResponse();
};
