/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   config.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mayeung <mayeung@student.42london.com>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/14 17:12:27 by mayeung           #+#    #+#             */
/*   Updated: 2025/11/16 17:29:49 by mayeung          ###   ########.fr       */
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

	locations["/"] = l;
	listenAddress = "127.0.0.1";
	serverName = "localhost";
	port = 8080;
}

Config::~Config()
{

}

const std::map<std::string, Location> &Config::getLocations()
{
	return locations;
}

const std::map<int, std::string>	&Config::getErrorPages()
{
	return errorPages;
}

const std::string	&Config::getListenAddress()
{
	return listenAddress;
}

const std::string	&Config::getServerName()
{
	return serverName;
}

const int	&Config::getPort()
{
	return port;
}
