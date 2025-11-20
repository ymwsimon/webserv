/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   config.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mayeung <mayeung@student.42london.com>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/14 17:12:27 by mayeung           #+#    #+#             */
/*   Updated: 2025/11/18 23:49:41 by mayeung          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../include/config.hpp"

// private:
// 		std::map<std::string, Location &>	locations;
// 		std::map<int, std::string> 			errorPages;
// 		std::string							listenAddress;
// 		std::string							serverName;
// 		int									port;
// 	public:
// 		Config();
// 		~Config();
// 		std::map<std::string, Location> &getLocations();
// 		std::map<int, std::string> 		&getErrorPages();
// 		std::string						&getListenAddress();
// 		std::string						&getServerName();
// 		int								&getPort();

Config::Config()
{
	Location	l;
	Location	l2;
	std::pair<std::vector<std::string>, std::string>	splitRes;

	locations.push_back(l);
	l2.setRouteStr("/test/");
	splitRes = splitPath(l2.getRouteStr());
	l2.setRoutePaths(splitRes.first);
	l2.setRootFolder("/folderb/");
	locations.push_back(l2);
	listenAddress = "127.0.0.1";
	serverName = "localhost";
	port = 8080;
}

Config::~Config()
{

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
	int			matchLength = -1;
	int			tmpRes;

	for (size_t i = 0; i < getLocations().size(); ++i)
	{
		tmpRes = getLocations()[i].getRouteMatchLength(paths);
		if (tmpRes > matchLength)
		{
			res = &getLocations()[i];
			matchLength = tmpRes;
		}
	}
	return res;
}
