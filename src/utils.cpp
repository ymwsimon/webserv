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
