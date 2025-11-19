/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   utils.hpp                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mayeung <mayeung@student.42london.com>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/12 19:36:33 by mayeung           #+#    #+#             */
/*   Updated: 2025/11/18 21:26:51 by mayeung          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once
#include <vector>
#include <algorithm>
#include <string>
#define BUFFER_SIZE 50000

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
