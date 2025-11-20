/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   location.cpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mayeung <mayeung@student.42london.com>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/14 17:01:00 by mayeung           #+#    #+#             */
/*   Updated: 2025/11/18 21:19:59 by mayeung          ###   ########.fr       */
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
	std::pair<std::vector<std::string>, std::string>	splitRes;

	routeStr = "/";
	splitRes = splitPath(routeStr);
	route = splitRes.first;
	rootFolder = "/data/www";
	indexPage = "index.html";
	allowedMethod = GET | POST;
	maxBodySize = 1024 * 1024 * 1024;
}

Location::~Location()
{

}

int	Location::getRouteMatchLength(const std::vector<std::string> &paths) const
{
	int	i = 0;

	if (route.empty())
		return 0;
	while (i < (int)route.size() && i < (int)paths.size() && route[i] == paths[i])
		++i;
	if (!i)
		return -1;
	return i;
}

const std::string	&Location::getRouteStr() const
{
	return routeStr;
}

const std::vector<std::string>	&Location::getRoute() const
{
	return route;
}

const std::string	&Location::getRootFolder() const
{
	return rootFolder;
}

void	Location::setRouteStr(std::string str)
{
	routeStr = str;
}

void	Location::setRoutePaths(std::vector<std::string> p)
{
	route = p;
}

void	Location::setRootFolder(std::string str)
{
	rootFolder = str;
}

void	Location::printLocation() const
{
	std::cout << "\t\tRoute:";
	for (size_t i = 0; i < route.size(); ++i)
		std::cout << " ,"[i != 0] << route[i];
	std::cout << std::endl;
	std::cout << "\t\tRoot folder: " << rootFolder << std::endl;
	std::cout << "\t\tIndex page: " << indexPage << std::endl;
}
