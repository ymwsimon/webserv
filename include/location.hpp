/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   location.hpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mayeung <mayeung@student.42london.com>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/13 14:22:07 by mayeung           #+#    #+#             */
/*   Updated: 2025/11/16 16:31:39 by mayeung          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once
#include <string>

class Location
{
	private:
		std::pair<std::string, std::string>	cgi;
		std::string							route;
		std::string							rootFolder;
		std::string							indexPage;
		std::string							uploadDir;
		std::string							redirect;
		int									allowedMethod;
		int									maxBodySize;
		bool								autoIndex;
	public:
		Location();
		~Location();
		enum method
		{
			GET = 1,
			POST = 2,
			DELETE = 4,
		};
		const std::string &getRoute();
};
