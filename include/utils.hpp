/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   utils.hpp                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mayeung <mayeung@student.42london.com>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/12 19:36:33 by mayeung           #+#    #+#             */
/*   Updated: 2025/11/22 21:31:01 by mayeung          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once
#include <vector>
#include <algorithm>
#include <string>
#include <iostream>
#include <sstream>
#include <dirent.h>
#define BUFFER_SIZE 50000
#define TRANSFER_SIZE 50000

typedef unsigned char Byte;
typedef std::vector<Byte> Bytes;

const Byte crlf[] = {'\r', '\n'};
const Bytes CRLF = Bytes(crlf, crlf + 2);
const Byte space[] = {' '};
const Bytes SPACE = Bytes(space, space + 1);
const Byte colon[] = {':'};
const Bytes COLON = Bytes(colon, colon + 1);
const Byte slash[] = {'/'};
const Bytes SLASH = Bytes(slash, slash + 1);
const Byte doubleSlash[] = {'/', '/'};
const Bytes DOUBLESLASH = Bytes(doubleSlash, doubleSlash + 2);
const Byte doubleCrlf[] = {'\r', '\n', '\r', '\n'};
const Bytes DOUBLECRLF = Bytes(doubleCrlf, doubleCrlf + 4);

Bytes::const_iterator	searchPattern(Bytes::const_iterator dataStart,
	Bytes::const_iterator dataEnd, Bytes::const_iterator patternStart, Bytes::const_iterator patternEnd);
Bytes::const_iterator	searchPattern(Bytes::const_iterator dataStart,
	Bytes::const_iterator dataEnd, const Bytes &pattern);
Bytes::const_iterator	searchPattern(const Bytes &data, const Bytes &pattern);

std::string	trim(std::string &str);

std::pair<std::vector<std::string>, std::string>	splitPath(const std::string &pathStr);

Bytes		staticPage();
Bytes		defaultErrorPage();
std::string	mergeFullPath(const std::string rootPath, const std::vector<std::string> &routePaths, const std::string &fileName);
bool		isDir(const std::string &filePath);
