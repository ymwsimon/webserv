/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   utils.hpp                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mayeung <mayeung@student.42london.com>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/12 19:36:33 by mayeung           #+#    #+#             */
/*   Updated: 2026/01/11 22:49:18 by mayeung          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once
#include <vector>
#include <algorithm>
#include <string>
#include <iostream>
#include <sstream>
#include <dirent.h>
#include <sys/stat.h>
#include <unistd.h>
#include "html.hpp"
#include "http.hpp"
#define BUFFER_SIZE 50000
#define TRANSFER_SIZE 50000

typedef unsigned char Byte;
typedef std::vector<Byte> Bytes;

const Byte crlf[] = {'\r', '\n'};
const Bytes CRLF = Bytes(crlf, crlf + 2);
const std::string CRLFStr = std::string(crlf, crlf + 2);
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

std::string::const_iterator	searchLastStr(const std::string &data, std::string pattern);
std::string	extractFileExt(std::string fullPath);

std::string	trim(std::string &str);

std::vector<std::string>	splitPath(const std::string &pathStr);

Bytes		staticPage();
Bytes		defaultErrorPage();
std::string	mergeFullPath(const std::string rootPath, const std::vector<std::string> &routePaths, bool splitPath);
bool		isDir(const std::string &filePath);
bool		isRegularFile(const std::string &filePath);
bool		fileExist(const std::string &filePath);
bool		fileReadOK(const std::string &filePath);
bool		fileExeOK(const std::string &filePath);
bool		fileWriteOK(const std::string &filePath);
bool		fileWithExt(const std::string &filePath, std::string ext);
off_t		fileSize(const std::string &filePath);
std::string	toString(int n);
std::string	stringToLowerCase(std::string str);
int			toInt(std::string str);
Bytes		&appendBuf(Bytes &bytes, const unsigned char *buf, size_t size);
Bytes		&appendBytes(Bytes &bytes, std::string toAppend);
Bytes		&appendBytes(Bytes &bytes, Bytes::const_iterator start, Bytes::const_iterator end);
void		printBytes(const Bytes &bytes);
Bytes		stringToBytes(std::string str);
std::string bytesToString(const Bytes &data);
std::string bytesToString(Bytes::const_iterator start, Bytes::const_iterator end);
void		logMessage(std::ostream &s, std::string msg);