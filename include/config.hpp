/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   config.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mayeung <mayeung@student.42london.com>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/12 21:02:21 by mayeung           #+#    #+#             */
/*   Updated: 2025/11/18 23:49:34 by mayeung          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once
#include <string>
#include <map>
#include "location.hpp"

class Config
{
	private:
		std::map<std::string, Location>	locations;
		std::map<int, std::string> 		errorPages;
		std::string						listenAddress;
		std::string						serverName;
		int								port;
	public:
		Config();
		~Config();
		const std::map<std::string, Location>	&getLocations() const;
		const std::map<int, std::string> 		&getErrorPages() const;
		const std::string						&getListenAddress() const;
		const std::string						&getServerName() const;
		const int								&getPort() const;
};
