/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   utils.cpp                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mayeung <mayeung@student.42london.com>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/16 16:29:26 by mayeung           #+#    #+#             */
/*   Updated: 2025/11/16 16:29:27 by mayeung          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../include/utils.hpp"

Bytes::const_iterator	searchPattern(const Bytes &data, const Bytes &pattern)
{
	return std::search(data.begin(), data.end(), pattern.begin(), pattern.end());
}
