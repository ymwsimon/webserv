/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   utils.hpp                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mayeung <mayeung@student.42london.com>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/12 19:36:33 by mayeung           #+#    #+#             */
/*   Updated: 2025/11/16 23:31:26 by mayeung          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once
#include <vector>
#include <algorithm>
#define BUFFER_SIZE 50000

typedef unsigned char Byte;
typedef std::vector<Byte> Bytes;

const Bytes CRLF = {'\r', '\n'};
const Bytes SPACE = {' '};
const Bytes COLON = {':'};
const Bytes DOUBLE_CRLF = {'\r', '\n', '\r', '\n'};

Bytes::const_iterator	searchPattern(Bytes::const_iterator dataStart,
	Bytes::const_iterator dataEnd, Bytes::const_iterator patternStart, Bytes::const_iterator patternEnd);
Bytes::const_iterator	searchPattern(Bytes::const_iterator dataStart,
	Bytes::const_iterator dataEnd, const Bytes &pattern);
Bytes::const_iterator	searchPattern(const Bytes &data, const Bytes &pattern);
