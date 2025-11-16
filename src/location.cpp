/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   location.cpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mayeung <mayeung@student.42london.com>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/14 17:01:00 by mayeung           #+#    #+#             */
/*   Updated: 2025/11/16 17:28:52 by mayeung          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../include/location.hpp"
// std::pair<std::string, std::string>	cgi;
// 		std::string							route;
// 		std::string							rootFolder;
// 		std::string							indexPage;
// 		std::string							uploadDir;
// 		std::string							redirect;
// 		int									allowedMethod;
// 		int									maxBodySize;
// 		bool								autoIndex;
Location::Location()
{
	route = "/";
	rootFolder = "/data/www";
	indexPage = "index.html";
	allowedMethod = GET | POST;
	maxBodySize = 1024 * 1024 * 1024;
}

Location::~Location()
{

}
