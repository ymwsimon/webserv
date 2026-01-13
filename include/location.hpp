/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   location.hpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mayeung <mayeung@student.42london.com>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/13 14:22:07 by mayeung           #+#    #+#             */
/*   Updated: 2026/01/12 22:30:02 by mayeung          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once
#include <string>
#include <vector>
#include <map>
#include <iostream>
#include <fstream>
#include "utils.hpp"

class Location
{
	private:
		std::map<std::string, std::string>	cgi;
		std::string							routeStr;
		std::vector<std::string>			route;
		std::string							rootFolder;
		std::vector<std::string>			indexPages;
		std::string							uploadDir;
		std::string							redirect;
		int									allowedMethod;
		int									maxBodySize;
		bool								autoIndex;
	public:
		enum method
		{
			GET = 1,
			POST = 2,
			DELETE = 4,
		};
		Location();
		Location(const Location &right);
		~Location();
		Location						&operator=(const Location &right);
		const std::string				&getRouteStr() const;
		const std::vector<std::string>	&getRoute() const;
		const std::string				&getRootFolder() const;
		const std::vector<std::string>	&getIndexPages() const;
		const std::string				&getuploadDir() const;
		bool							getAutoIndex() const;
		bool							hasCGIConfig() const;
		int								getAllowedMethod() const;
		int								getMaxBodySize() const;
		void							setRouteStr(std::string str);
		void							setRoutePaths(std::vector<std::string> p);
		void							setRootFolder(std::string str);
		void							printLocation() const;
		int								getRouteMatchLength(const std::vector<std::string> &paths) const;
		std::string						findValidIndexPage(std::string &folderPathStr) const;
		std::string						findCGIExecutable(std::string ext) const;
		Bytes							generateIndexPages(std::string &folderPathStr, std::string routePath) const;
		const std::map<std::string, std::string>	&getCGIConfig() const;
		bool							isOneOfCGIConfig(std::string &filePath) const;
		bool							isResourceReachable(const std::string &rootPath,
			const std::vector<std::string> &routePaths, const std::string &fileName) const;
};
