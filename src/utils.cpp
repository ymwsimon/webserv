/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   utils.cpp                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mayeung <mayeung@student.42london.com>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/16 16:29:26 by mayeung           #+#    #+#             */
/*   Updated: 2025/11/17 14:42:59 by mayeung          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../include/utils.hpp"

Bytes::const_iterator	searchPattern(Bytes::const_iterator dataStart,
	Bytes::const_iterator dataEnd, Bytes::const_iterator patternStart, Bytes::const_iterator patternEnd)
{
	return std::search(dataStart, dataEnd, patternStart, patternEnd);
}

Bytes::const_iterator	searchPattern(Bytes::const_iterator dataStart,
	Bytes::const_iterator dataEnd, const Bytes &pattern)
{
	return std::search(dataStart, dataEnd, pattern.begin(), pattern.end());
}

Bytes::const_iterator	searchPattern(const Bytes &data, const Bytes &pattern)
{
	return std::search(data.begin(), data.end(), pattern.begin(), pattern.end());
}

std::string	trim(std::string &str)
{
	str.erase(str.find_last_not_of(' ') + 1);
	str.erase(0, str.find_first_not_of(' '));
	return str;
}

std::pair<std::vector<std::string>, std::string>	splitPath(const std::string &pathStr)
{
	std::string					resFileName;
	std::vector<std::string>	resPath;
	std::string::const_iterator	it = pathStr.begin();
	std::string::const_iterator	endIt;

	try
	{
		if (!pathStr.empty() && *it == '/')
			++it;
		endIt = std::find(it, pathStr.end(), '/');
		while (it != pathStr.end() && endIt != pathStr.end())
		{
			resPath.push_back(std::string(it, endIt));
			it = endIt + 1;
			endIt = std::find(endIt + 1, pathStr.end(), '/');
		}
		resFileName = std::string(pathStr.rbegin(), std::find(pathStr.rbegin(), pathStr.rend(), '/'));
		std::reverse(resFileName.begin(), resFileName.end());
	}
	catch (std::exception &e)
	{
		std::cerr << e.what();
	}
	return std::make_pair(resPath, resFileName);
}

std::string	replaceDoubleSlash(std::string &input)
{
	std::string	res;

	(void)input;
	return res;
}
