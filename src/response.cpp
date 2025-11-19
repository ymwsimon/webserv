/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   response.cpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mayeung <mayeung@student.42london.com>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/17 14:05:04 by mayeung           #+#    #+#             */
/*   Updated: 2025/11/19 00:23:08 by mayeung          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../include/response.hpp"

Response::Response(const Response &other) : service(other.service), request(other.request)
{
	// pageStream = new std::ifstream("", std::ios_base::in);
}

Response::Response(Service &ser, Request &req) : service(ser), request(req)
{

}

Response::~Response()
{

}

Response	&Response::operator=(const Response &right)
{
	////////////////////////////////////////need to implement
	service = right.service;
	request = right.request;
	pageStream = right.pageStream;
	return *this;
}
