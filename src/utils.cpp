/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   utils.cpp                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mayeung <mayeung@student.42london.com>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/16 16:29:26 by mayeung           #+#    #+#             */
/*   Updated: 2026/01/11 22:49:26 by mayeung          ###   ########.fr       */
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

std::string::const_iterator	searchLastStr(const std::string &data, std::string pattern)
{
	return std::find_end(data.begin(), data.end(), pattern.begin(), pattern.end());
}

std::string	extractFileExt(std::string fullPath)
{
	std::string	res;
	std::string	fileName;
	size_t		pos;

	pos = fullPath.find_last_of('/');
	std::cout << "/ pos" << pos << std::endl;
	if (pos != std::string::npos)
	{
		fileName = fullPath.substr(pos);
		pos = fileName.find_last_of('.');
		std::cout << ". pos" << pos << std::endl;
		if (pos != std::string::npos)
			return fileName.substr(pos + 1);
	}
	return "";
}

std::string	trim(std::string &str)
{
	str.erase(str.find_last_not_of(' ') + 1);
	str.erase(0, str.find_first_not_of(' '));
	return str;
}

std::vector<std::string>	splitPath(const std::string &pathStr)
{
	std::vector<std::string>	resPath;
	std::string::const_iterator	it = pathStr.begin();
	std::string::const_iterator	endIt;

	while (it != pathStr.end())
	{
		++it;
		endIt = std::find(it, pathStr.end(), '/');
		resPath.push_back(std::string(it, endIt));
		it = endIt;
	}
	return resPath;
}

std::string	mergeFullPath(const std::string rootPath, const std::vector<std::string> &routePaths, bool splitPath)
{
	std::string	res = rootPath;

	for (size_t i = 0; i < routePaths.size(); ++i)
	{
		res += "/" + routePaths[i];
		if (splitPath && isRegularFile(res))
			break ;
	}
	return res;
}

bool	isDir(const std::string &filePath)
{
	struct stat	fileStat;

	if (!stat(filePath.c_str(), &fileStat))
		return S_ISDIR(fileStat.st_mode);
	return false;
}

bool	isRegularFile(const std::string &filePath)
{
	struct stat	fileStat;

	if (!stat(filePath.c_str(), &fileStat))
		return S_ISREG(fileStat.st_mode);
	return false;
}

bool	fileExist(const std::string &filePath)
{
	return (access(filePath.c_str(), F_OK) == 0);
}

bool	fileReadOK(const std::string &filePath)
{
	return (access(filePath.c_str(), F_OK | R_OK) == 0);
}

bool	fileExeOK(const std::string &filePath)
{
	return (access(filePath.c_str(), F_OK | X_OK) == 0);
}

bool	fileWriteOK(const std::string &filePath)
{
	return (access(filePath.c_str(), F_OK | W_OK) == 0);
}

bool	fileWithExt(const std::string &filePath, std::string ext)
{
	std::string::const_reverse_iterator	ri;

	if (filePath.length() > ("." + ext).length())
	{
		ri = filePath.rbegin();
		for (std::string::const_reverse_iterator r = ext.rbegin(); r != ext.rend(); ++r, ++ri)
		{
			if (*ri != *r)
				return false;
		}
		return *ri == '.';
	}
	return false;
}

off_t	fileSize(const std::string &filePath)
{
	struct stat	fileInfo;

	if (stat(filePath.c_str(), & fileInfo) == 0)
		return fileInfo.st_size;
	return 0;
}

std::string	toString(int n)
{
	std::stringstream	ss;

	ss << n;
	return ss.str();
}

std::string	stringToLowerCase(std::string str)
{
	std::transform(str.begin(), str.end(), str.begin(), ::towlower);
	return str;
}

int	toInt(std::string str)
{
	std::stringstream	ss(str);
	int					res;

	try
	{
		ss >> res;
	}
	catch(const std::exception& e)
	{
		return -1;
	}
	return res;
}

Bytes	&appendBuf(Bytes &bytes, const unsigned char *buf, size_t size)
{
	for (size_t i = 0; i < size; ++i)
		bytes.push_back(buf[i]);
	return bytes;
}

Bytes	&appendBytes(Bytes &bytes, std::string toAppend)
{
	for (size_t i = 0; i < toAppend.length(); ++i)
		bytes.push_back(toAppend[i]);
	return bytes;
}

Bytes	&appendBytes(Bytes &bytes, Bytes::const_iterator start, Bytes::const_iterator end)
{
	for (; start != end; ++start)
		bytes.push_back(*start);
	return bytes;
}

void	printBytes(const Bytes &bytes)
{
	for (size_t i = 0; i < bytes.size(); ++i)
		std::cout << bytes[i];
}

Bytes	stringToBytes(std::string str)
{
	return Bytes(str.begin(), str.end());
}

std::string bytesToString(const Bytes &data)
{
	std::string	res;

	for (Bytes::const_iterator it = data.begin(); it != data.end(); ++it)
		res.push_back(*it);
	return res;
}

std::string bytesToString(Bytes::const_iterator start, Bytes::const_iterator end)
{
	std::string	res;

	for (; start != end; ++start)
		res.push_back(*start);
	return res;
}

void	logMessage(std::ostream &s, std::string msg)
{
	s << msg << std::endl;
}
