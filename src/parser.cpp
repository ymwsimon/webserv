#include "../include/parser.hpp"

bool	parseKeyword(std::stringstream &ss, std::string keyword, char delim)
{
	std::string	str;

	std::getline(ss, str, delim);
	trim(str);
	if (str != keyword)
		return false;
	return true;
}

bool	parseKeyword(std::stringstream &ss, std::string keyword)
{
	return parseKeyword(ss, keyword, '\n');
}

bool	extractUntil(std::stringstream &ss, std::string &res, char delim)
{
	std::getline(ss, res, delim);
	trim(res);
	return !res.empty();
}

bool	lineEndWith(std::stringstream &ss, std::string &str, char end)
{
	std::getline(ss, str);
	trim(str);
	if (str.empty() || *str.rbegin() != end)
		return false;
	str.erase(str.length() - 1);
	trim(str);
	return true;
}

bool	readConfigFile(std::string fileName, std::string &str)
{
	char	buf[(BUFFER_SIZE * 2)];

	if (!fileExist(fileName) || !fileWithExt(fileName, "conf") || fileSize(fileName) > (BUFFER_SIZE * 2))
		return false;
	std::ifstream		file(fileName.c_str(), std::ios::in);

	if (file.bad())
	{
		file.close();
		return false;
	}
	file.read(buf, (BUFFER_SIZE * 2));
	if (file.gcount() > 0)
		str.append(buf, file.gcount());
	file.close();
	return true;
}

void	putStrBack(std::stringstream &ss, std::string str)
{
	for (size_t i = str.length(); i;)
		ss.putback(str[--i]);
}

bool	httpMethodParser(std::string str, Location &res)
{
	size_t	i = 0;
	size_t	e;
	int		m = 0;

	while (i < str.length())
	{
		e = str.find('|', i);
		if (e == std::string::npos)
			e = str.length();
		std::string	method(str, i, e - i);

		trim(method);
		if (method == "GET" && m & GET)
			return false;
		if (method == "POST" && m & POST)
			return false;
		if (method == "DELETE" && m & DELETE)
			return false;
		if (method == "GET")
			m |= GET;
		if (method == "POST")
			m |= POST;
		if (method == "DELETE")
			m |= DELETE;
		i = e + 1;
	}
	res.setAllowedMethod(m);
	return true;
}

bool	locationParseOk(std::set<std::string> &seen)
{
	return seen.count("root");
}

bool	configParseOk(std::set<std::string> &seen)
{
	return seen.count("listen") && seen.count("location");
}

bool	locationContentParser(std::stringstream &ss, Location &res, std::set<std::string> &seen,
	std::string &str)
{
	std::getline(ss, str, ' ');
	trim(str);
	std::cout <<"first str:" << str <<std::endl;
	if (str.empty())
		return true;
	if (str == "cgi")
	{
		if (!lineEndWith(ss, str, ';'))
			return false;
		size_t		pos = str.find_first_of(' ');
		std::string	ext = str.substr(0, pos);
		std::string	path = str.substr(pos, str.length());

		trim(ext);
		trim(path);		
		if (pos == std::string::npos)
			return false;
		if (res.getCGIConfig().count(ext))
			return false;
		if (ext.empty() || path.empty())
			return false;
		res.addCgiSetting(ext, path);
	}
	else if (str == "root" && !seen.count(str))
	{
		seen.insert(str);
		if (!lineEndWith(ss, str, ';'))
			return false;
		if (str.empty() || *str.begin() != '/')
			return false;
		res.setRootFolder(str);
	}
	else if (str == "index")
	{
		if (!lineEndWith(ss, str, ';'))
			return false;
		res.addIndexPage(str);
	}
	else if (str == "redirect" && !seen.count(str))
	{
		seen.insert(str);
		if (!lineEndWith(ss, str, ';'))
			return false;
		if (!stringStartWith(str, "http://"))
			return false;
		res.setRedirect(str);
	}
	else if (str == "allowed_method" && !seen.count(str))
	{
		seen.insert(str);
		if (!lineEndWith(ss, str, ';'))
			return false;
		if (!httpMethodParser(str, res))
			return false;
		if (!httpMethodParser(str, res))
			return false;
	}
	else if (str == "max_body_size" && !seen.count(str))
	{
		seen.insert(str);
		if (!lineEndWith(ss, str, ';'))
			return false;
		long long	size = toInt(str);

		if (str.empty() || !digitOnly(str))
			return false;
		if (size < 0)
			return false;
		if (size > BODY_SIZE_LIMIT)
			return false;
		res.setMaxBodySize(size);
	}
	else if (str == "autoindex" && !seen.count(str))
	{
		seen.insert(str);
		if (!lineEndWith(ss, str, ';'))
			return false;
		if (str == "on")
			res.setAutoIndex(true);
		else if (str == "off")
			res.setAutoIndex(false);
		else
			return false;
	}
	else
		return false;
	return true;
}

bool	locationParser(std::stringstream &ss, Location &res, std::string &str)
{
	std::set<std::string>	seenKeyword;

	if (!parseKeyword(ss, "location", ' '))
		return false;
	if (!extractUntil(ss, str, ' '))
		return false;
	res.setRouteStr(str);
	res.setRoutePaths(splitPath(str));
	if (!parseKeyword(ss, "{"))
		return false;
	while (locationContentParser(ss, res, seenKeyword, str))
		{}
	std::cout << "location final str:"<<str << std::endl;
	if (str.empty() || str[0] != '}')
		return false;
	str.erase(0, 1);
	str.push_back(' ');
	putStrBack(ss, str);
	// trim(str);
	return locationParseOk(seenKeyword);
}

bool	ipAddrPartOk(std::string &str, size_t &start, size_t &pos, bool findDot)
{
	std::string	valStr;
	int			val;

	if (findDot)
	{
		pos = str.find('.', start);
		if (pos == std::string::npos)
			return false;
	}
	else
		pos = str.size();
	valStr = str.substr(start, pos - start);
	if (!digitOnly(valStr))
		return false;
	val = toInt(valStr);
	if (val < 0 || val > 255)
		return false;
	start = pos + 1;
	return true;
}

bool	ipAddressStrOk(std::string str)
{
	size_t		start = 0;
	size_t		pos = 0;

	if (!ipAddrPartOk(str, start, pos, true))
		return false;
	if (!ipAddrPartOk(str, start, pos, true))
		return false;
	if (!ipAddrPartOk(str, start, pos, true))
		return false;
	if (!ipAddrPartOk(str, start, pos, false))
		return false;
	return true;
}

bool	configContentParser(std::stringstream &ss, Config &res, std::set<std::string> &seen,
	std::string &str)
{
	std::getline(ss, str, ' ');
	trim(str);
	std::cout <<"confgi content first str:" << str <<std::endl;
	if (str.empty())
		return true;
	if (str == "error_page")
	{
		if (!lineEndWith(ss, str, ';'))
			return false;
		size_t		pos = str.find_first_of(' ');
		std::string	codeStr = str.substr(0, pos);
		int			code;
		std::string	path = str.substr(pos, str.length());

		trim(codeStr);
		trim(path);
		code = toInt(codeStr);
		if (pos == std::string::npos)
			return false;
		if (!digitOnly(codeStr))
			return false;
		if (codeStr.empty() || path.empty())
			return false;
		if (!res.addErrorPage(code, path))
			return false;
	}
	else if (str == "listen" && !seen.count(str))
	{
		seen.insert(str);
		size_t	pos;
	
		if (!lineEndWith(ss, str, ';'))
			return false;

		pos = str.find_first_of(':');
		if (pos == std::string::npos)
			return false;
		std::string	ipAddress = str.substr(0, pos);
		std::string	portStr = str.substr(pos + 1, str.length());
		int			port;
		trim(ipAddress);
		trim(portStr);
		if (ipAddress.empty() || portStr.empty())
			return false;
		if (!ipAddressStrOk(ipAddress))
			return false;
		port = toInt(portStr);
		if (port <= 0 || port > 65535)
			return false;
		res.setListenAddress(ipAddress);
		res.setPort(port);
	}
	else if (str == "location")
	{
		seen.insert(str);
		Location	l;

		putStrBack(ss, "location ");
		if (!locationParser(ss, l, str))
			return false;
		if (!res.addLocation(l))
			return false;
		// l.printLocation();
	}
	else
		return false;
	return true;
}

bool	configParser(std::stringstream &ss, Config &res, std::string &str)
{
	std::set<std::string>	seenKeyword;

	if (!parseKeyword(ss, "server", ' '))
		return false;
	if (!parseKeyword(ss, "{"))
		return false;
	while (configContentParser(ss, res, seenKeyword, str))
		{}
	std::cout << "config final str:"<< str << std::endl;
	if (str[0] != '}')
		return false;
	if (!configParseOk(seenKeyword))
		return false;
	str.erase(0, 1);
	// trim(str);
	if (!str.empty())
	{
		str.push_back(' ');
		putStrBack(ss, str);
	}
	return true;
}

bool	serverConfigParser(Server &res, std::string configFileStr)
{
	size_t				i = 1;
	std::stringstream	ss;
	std::string			str;

	if (configFileStr.empty())
		return false;
	ss << configFileStr;
	while (!ss.eof())
	{
		Config	c;

		if (!configParser(ss, c, str))
		{
			trim(str);
			if (!str.empty())
				return false;
		}
		else
			res.addConfig(c);
		std::cout << "i:" << i << std::endl;
		// putStrBack(ss, str);
		++i;
	}
	return true;
}
