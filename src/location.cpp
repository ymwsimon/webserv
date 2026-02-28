/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   location.cpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mayeung <mayeung@student.42london.com>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/14 17:01:00 by mayeung           #+#    #+#             */
/*   Updated: 2026/02/27 11:29:58 by mayeung          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../include/location.hpp"

Location::Location()
{
	autoIndex = false;
	allowedMethod = 0;
	maxBodySize = __INT_MAX__;
	// std::vector<std::string>	splitRes;

	// routeStr = "/";
	// splitRes = splitPath(routeStr);
	// route = splitRes;
	// rootFolder = "./data/www";
	// indexPages.push_back("index.html");
	// indexPages.push_back("b.html");
	// allowedMethod = GET;
	// // allowedMethod = GET | POST | DELETE;
	// maxBodySize = 1024 * 1024 * 1024;
	// autoIndex = true;
	// cgi.insert(std::make_pair("php", "/usr/bin/php-cgi"));
	// cgi.insert(std::make_pair("bla", "/home/user/cpp/webserv/cgi_tester"));
	// cgi.insert(std::make_pair("html", "aaa"));
}

Location::Location(int i)
{
	if (i == 1)
	{
		std::vector<std::string>	splitRes;

		routeStr = "/";
		splitRes = splitPath(routeStr);
		route = splitRes;
		rootFolder = "/home/user/cpp/webserv/data/www";
		// indexPages.push_back("index.html");
		// indexPages.push_back("b.html");
		// allowedMethod = GET | POST;
		allowedMethod = GET;
		// allowedMethod = GET | POST | DELETE;
		maxBodySize = 1024 * 1024 * 1024;
		autoIndex = true;
		cgi.insert(std::make_pair("php", "/usr/bin/php-cgi"));
		cgi.insert(std::make_pair("bla", "/home/user/cpp/webserv/cgi_tester"));
		// cgi.insert(std::make_pair("bla", "/home/mayeung/42/commoncore/webserv/cgi_tester"));
		// cgi.insert(std::make_pair("html", "aaa"));
		// redirect = "http://google.com";
	}
	else if (i == 2)
	{
		std::vector<std::string>	splitRes;

		routeStr = "/post_body/";
		splitRes = splitPath(routeStr);
		route = splitRes;
		rootFolder = "/home/user/cpp/webserv/data/www";
		indexPages.push_back("index.html");
		indexPages.push_back("b.html");
		allowedMethod = POST;
		// allowedMethod = GET | POST | DELETE;
		maxBodySize = 100;
		autoIndex = false;
		cgi.insert(std::make_pair("php", "/usr/bin/php-cgi"));
		cgi.insert(std::make_pair("bla", "/home/user/cpp/webserv/cgi_tester"));
		// cgi.insert(std::make_pair("bla", "/home/mayeung/42/commoncore/webserv/cgi_tester"));
		// cgi.insert(std::make_pair("html", "aaa"));
	}
	else if (i == 3)
	{
		std::vector<std::string>	splitRes;

		routeStr = "/directory/";
		splitRes = splitPath(routeStr);
		route = splitRes;
		rootFolder = "/home/user/cpp/webserv/data/www/YoupiBanane";
		indexPages.push_back("youpi.bad_extension");
		allowedMethod = GET | POST;
		maxBodySize = (size_t)1024 * 1024 * 1024 * 2;
		autoIndex = false;
		cgi.insert(std::make_pair("php", "/usr/bin/php-cgi"));
		cgi.insert(std::make_pair("bla", "/home/user/cpp/webserv/cgi_tester"));
		// cgi.insert(std::make_pair("bla", "/home/mayeung/42/commoncore/webserv/cgi_tester"));
		// cgi.insert(std::make_pair("html", "aaa"));
	}
}

Location::Location(const Location &right)
{
	*this = right;
}

Location::~Location()
{

}

Location	&Location::operator=(const Location &right)
{
	if (this != &right)
	{
		cgi = right.cgi;
		routeStr = right.routeStr;
		route = right.route;
		rootFolder = right.rootFolder;
		indexPages = right.indexPages;
		redirect = right.redirect;
		allowedMethod = right.allowedMethod;
		maxBodySize = right.maxBodySize;
		autoIndex = right.autoIndex;
	}
	return *this;
}

int	Location::getRouteMatchLength(const std::vector<std::string> &paths) const
{
	int	i = 0;

	if (route.size() == 1 && route[0].empty())
		return 0;
	while (i < (int)route.size() && i < (int)paths.size() && route[i] == paths[i])
		++i;
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

bool	Location::getAutoIndex() const
{
	return autoIndex;
}

bool	Location::hasCGIConfig() const
{
	return !cgi.empty();
}

int	Location::getAllowedMethod() const
{
	return allowedMethod;
}

int	Location::getMaxBodySize() const
{
	return maxBodySize;
}

std::string	Location::getRedirect() const
{
	return redirect;
}

void	Location::setRoutePaths(std::vector<std::string> p)
{
	route = p;
}

void	Location::setRootFolder(std::string str)
{
	rootFolder = str;
}

void	Location::setRedirect(std::string str)
{
	redirect = str;
}

void	Location::setAllowedMethod(int m)
{
	allowedMethod = m;
}

void	Location::setMaxBodySize(int size)
{
	maxBodySize = size;
}

void	Location::setAutoIndex(bool b)
{
	autoIndex = b;
}

void	Location::addIndexPage(std::string str)
{
	indexPages.insert(indexPages.end(), str);
}

void	Location::addCgiSetting(std::string ext, std::string path)
{
	cgi.insert(std::make_pair(ext, path));
}

void	Location::printLocation() const
{
	std::cout << "\t\tRoute:";
	for (size_t i = 0; i < route.size(); ++i)
		std::cout << " ,"[i != 0] << route[i];
	std::cout << std::endl;
	std::cout << "\t\tRoot folder: " << rootFolder << std::endl;
	std::cout << "\t\tRoute str: " << routeStr << std::endl;
	std::cout << "\t\tAuto index:" << autoIndex << std::endl;
	std::cout << "\t\tRedirect:" << redirect << std::endl;
	std::cout << "\t\tAllowed method:" << allowedMethod << std::endl;
	std::cout << "\t\tMax body size:" << maxBodySize << std::endl;
	std::cout << "\t\tIndex page:";
	for (size_t i = 0; i < indexPages.size(); ++i)
		std::cout << " " << indexPages[i];
	std::cout << std::endl;
	std::cout << "\t\tCGI:";
	for (std::map<std::string, std::string>::const_iterator i = cgi.begin(); i != cgi.end(); ++i)
		std::cout << i->first << " " << i->second << std::endl;
	std::cout << std::endl;
}

std::string	Location::findValidIndexPage(std::string &folderPathStr) const
{
	std::string	fullPath;

	for (size_t i = 0; i < indexPages.size(); ++i)
	{
		fullPath = folderPathStr + indexPages[i];
		std::cout << "try opening " << fullPath << std::endl;
		if (isRegularFile(fullPath) || fileExist(fullPath) || isOneOfCGIConfig(fullPath))
			return fullPath;
		std::cout << "open fail" << std::endl;
	}
	return "";
}

std::string	Location::findCGIExecutable(std::string ext) const
{
	if (cgi.count(ext) > 0)
		return cgi.at(ext);
	return "";
}

Bytes	Location::generateIndexPages(std::string &folderPathStr, std::string routePath) const
{
	DIR					*dir = NULL;
	struct dirent		*dirEntry;
	std::string			res;
	std::string			body;
	std::string			tmp;

	std::cout << "try to open dir " << folderPathStr << std::endl;
	dir = opendir(folderPathStr.c_str());
	if (dir)
	{
		std::cout << "generating index page.." << std::endl;
		for (dirEntry = readdir(dir); dirEntry; dirEntry = readdir(dir))
		{
			// std::cout << "reading .. path: ";
			// std::cout << dirEntry->d_name << std::endl;
			tmp = std::string(dirEntry->d_name);
			body += appendHtmlTag("p", tmp);
		}
		closedir(dir);
		tmp = routePath;
		body = appendHtmlTag("h1", tmp) + body;
		appendHtmlTag(BODY, body);
		tmp = "Index of " + routePath;
		appendHtmlTag(TITLE, tmp);
		appendHtmlTag(HEADTag, tmp);
		res = tmp + body;
		appendHtmlTag(HTML, res);
		res += genHtmlTagStart(DOCTYPE + " " + HTML);
		res = genHttpResponse(200, res);
	}
	// std::cout << "res page" << std::endl << res << std::endl
		// << "\tsize:" << std::distance(res.begin(), res.end()) << std::endl;
	return Bytes(res.begin(), res.end());
}

const std::map<std::string, std::string>	&Location::getCGIConfig() const
{
	return cgi;
}

bool	Location::isOneOfCGIConfig(std::string &filePath) const
{
	std::string	ext = extractFileExt(filePath);

	return !ext.empty() && cgi.count(ext) > 0;
}

bool	Location::isMethodAllowed(std::string method) const
{
	return ((method == "GET" && (allowedMethod & GET))
		|| (method == "POST" && (allowedMethod & POST))
		|| (method == "DELETE" && (allowedMethod & DELETE))
		|| (method == "PUT" && (allowedMethod & PUT))
		|| (method == "HEAD" && (allowedMethod & HEAD)));
}
