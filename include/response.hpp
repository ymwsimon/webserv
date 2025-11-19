/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   response.hpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mayeung <mayeung@student.42london.com>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/13 18:45:26 by mayeung           #+#    #+#             */
/*   Updated: 2025/11/19 00:19:45 by mayeung          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once
#include <vector>
#include <fstream>
#include "service.hpp"
#include "request.hpp"

class Response
{
	private:
		Service			&service;
		Request			&request;
		std::ifstream	*pageStream;
		Response();
	public:
		Response(const Response &other);
		Response(Service &ser, Request &req);
		~Response();
		Response	&operator=(const Response &right);
};
