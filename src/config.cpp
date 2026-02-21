/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   config.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mayeung <mayeung@student.42london.com>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/14 17:12:27 by mayeung           #+#    #+#             */
/*   Updated: 2026/02/21 17:24:50 by mayeung          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../include/config.hpp"

Config::Config()
{
	Location	l(errorPages);
	Location	l2(errorPages);
	Location	l3(errorPages);
	std::vector<std::string>	splitRes;

	// locations.push_back(l);
	l2.setRouteStr("/test/");
	splitRes = splitPath(l2.getRouteStr());
	l2.setRoutePaths(splitRes);
	l2.setRootFolder("/folderb/");
	// locations.push_back(l2);
	locations.push_back(Location(errorPages, 1));
	locations.push_back(Location(errorPages, 2));
	locations.push_back(Location(errorPages, 3));
	listenAddress = "127.0.0.1";
	serverName = "localhost";
	port = 8080;
}

Config::Config(const Config &right)
{
	*this = right;
}

Config::~Config()
{

}

Config	&Config::operator=(const Config &right)
{
	if (this != &right)
	{
		errorPages = right.errorPages;
		locations = right.locations;
		listenAddress = right.listenAddress;
		serverName = right.serverName;
		port = right.port;
	}
	return *this;
}

const std::vector<Location> &Config::getLocations() const
{
	return locations;
}

const std::map<int, std::string>	&Config::getErrorPages() const
{
	return errorPages;
}

const std::string	&Config::getListenAddress() const
{
	return listenAddress;
}

const std::string	&Config::getServerName() const
{
	return serverName;
}

const int	&Config::getPort() const
{
	return port;
}

const Location	*Config::getLocationMatch(const std::vector<std::string> &paths) const
{
	const Location	*res = NULL;
	int				matchLength = -1;
	int				tmpRes;

	for (size_t i = 0; i < locations.size(); ++i)
	{
		tmpRes = locations[i].getRouteMatchLength(paths);
		if (tmpRes > matchLength)
		{
			res = &locations[i];
			matchLength = tmpRes;
		}
	}
	return res;
}
