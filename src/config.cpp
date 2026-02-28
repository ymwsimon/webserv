/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   config.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mayeung <mayeung@student.42london.com>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/14 17:12:27 by mayeung           #+#    #+#             */
/*   Updated: 2026/02/27 15:45:56 by mayeung          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../include/config.hpp"

Config::Config()
{
	port = -1;
	// Location	l;
	// Location	l2;
	// Location	l3;
	// std::vector<std::string>	splitRes;

	// // locations.push_back(l);
	// l2.setRouteStr("/test/");
	// splitRes = splitPath(l2.getRouteStr());
	// l2.setRoutePaths(splitRes);
	// l2.setRootFolder("/folderb/");
	// // locations.push_back(l2);
	// locations.push_back(Location(1));
	// locations.push_back(Location(2));
	// locations.push_back(Location(3));
	// listenAddress = "127.0.0.1";
	// port = 8080;
	// errorPages.insert(std::make_pair(404, "/q"));
	// errorPages.insert(std::make_pair(302, "/a"));
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

void	Config::setListenAddress(std::string str)
{
	listenAddress = str;
}

void	Config::setPort(int p)
{
	port = p;
}

void	Config::printConfig()
{
	std::cout << "Config:" << std::endl;
	std::cout << "\tListen addr:" << listenAddress << std::endl;
	std::cout << "\tListen port:" << port << std::endl;
	for (std::map<int, std::string>::const_iterator i = errorPages.begin(); i != errorPages.end(); ++i)
		std::cout << "\t\terror code:" << i->first << " path:" << i->second << std::endl;
	for (size_t i = 0; i < locations.size(); ++i)
		locations[i].printLocation();
	std::cout << std::endl;
}

bool	Config::addErrorPage(int code, std::string path)
{
	if ((code / 100 == 2) || code < 200 || code > 999)
		return false;
	if (errorPages.count(code))
		return false;
	errorPages.insert(std::make_pair(code, path));
	return true;
}

bool	Config::addLocation(Location &l)
{
	for (size_t i = 0; i < locations.size(); ++i)
		if (l.getRouteStr() == locations[i].getRouteStr())
			return false;
	locations.push_back(l);
	return true;
}
