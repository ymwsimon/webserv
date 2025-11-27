/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   location.cpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mayeung <mayeung@student.42london.com>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/14 17:01:00 by mayeung           #+#    #+#             */
/*   Updated: 2025/11/27 11:43:03 by mayeung          ###   ########.fr       */
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
	rootFolder = "./data/www";
	indexPages.push_back("index.html");
	indexPages.push_back("b.html");
	allowedMethod = GET | POST;
	maxBodySize = 1024 * 1024 * 1024;
	autoIndex = true;
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

const std::vector<std::string>	&Location::getIndexPages() const
{
	return indexPages;
}

const std::string	&Location::getuploadDir() const
{
	return uploadDir;
}

bool	Location::getAutoIndex() const
{
	return autoIndex;
}

int	Location::getAllowedMethod() const
{
	return allowedMethod;
}

int	Location::getMaxBodySize() const
{
	return maxBodySize;
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
	std::cout << "\t\tIndex page:";
	for (size_t i = 0; i < indexPages.size(); ++i)
		std::cout << " " << indexPages[i];
	std::cout << std::endl;
}

std::ifstream	*Location::tryOpenIndexPages(std::string &folderPathStr) const
{
	std::ifstream	*file = NULL;
	std::string		fullPath;

	for (size_t i = 0; i < indexPages.size() && !file; ++i)
	{
		fullPath = folderPathStr + indexPages[i];
		try
		{
			std::cout << "try opening " << fullPath << std::endl;
			file = new std::ifstream(fullPath.c_str(), std::ios_base::in);
			if (!file)
			{
				std::cout << "file stream pointer is null\n";
			}
			else if (!file->good())
			{
				std::cout << "open fail\n";
				file = NULL;
			}
		}
		catch (std::exception &e)
		{
			std::cout << "can't open " << fullPath << std::endl;
			file = NULL;
		}
	}
	return file;
}

Bytes	Location::generateIndexPages(std::string &folderPathStr, std::string routePath) const
{
	DIR					*dir = NULL;
	struct dirent		*dirEntry;
	std::string			res;
	std::string			str;
	std::stringstream	strStream;

	std::cout << "try to open dir " << folderPathStr << std::endl;
	dir = opendir(folderPathStr.c_str());
	if (dir)
	{
		std::cout << "generating index page..\n";
		res = "<!DOCTYPE html>"
				"<html>"
    			"<head>"
				"<meta content=\"text/html; charset=ISO-8859-1\" http-equiv=\"content-type\">"
				"<title>Index of " + routePath + "</title>"
				"</head>"
				"<body>"
				"<h1>" + routePath + "</h1>";

		for (dirEntry = readdir(dir); dirEntry; dirEntry = readdir(dir))
		{
			std::cout << "reading .. path: ";
			std::cout << dirEntry->d_name << std::endl;
			res += "<p>" + std::string(dirEntry->d_name) + "</p>";
		}
		closedir(dir);
		res += "</body></html>";
		str = "HTTP/1.1 200 OK\r\n"
				"Content-Type: text/html\r\n"
				"Content-Length: ";
		strStream << res.size();
		str += strStream.str();
		str += "\r\n\r\n";
		res = str + res;
	}
	std::cout << "res page\n" << res << " \n\tsize:" << std::distance(res.begin(), res.end()) << std::endl;
	return Bytes(res.begin(), res.end());
}

// bool	Location::isResourceReachable(const std::string &rootPath,
// 	const std::vector<std::string> &routePaths, const std::string &fileName) const
// {
// 	std::string	filePathStr;

// 	filePathStr = mergeFullPath(rootPath, routePaths, fileName);
// 	if (isDir(filePathStr))
// 	{
// 		if (autoIndex)
// 			return true;
		
// 	}
// 	else
// 	{

// 	}
// 	return false;
// }
