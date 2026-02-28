/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parser.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mayeung <mayeung@student.42london.com>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/12 21:11:33 by mayeung           #+#    #+#             */
/*   Updated: 2026/02/27 22:58:08 by mayeung          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once
// #include "utils.hpp"
#include "config.hpp"
#include "client.hpp"
#include "service.hpp"
#include "server.hpp"
#include <sstream>
// class Parser
// {
// 	private:
// 	public:
// 		Parser();
// 		~Parser();
// };

class Server;

bool	readConfigFile(const char *fileName, std::string &str);
bool	locationParser(std::stringstream &ss, Location &res, std::string &str);
bool	configParser(std::stringstream &ss, Config &res, std::string &str);
bool	serverConfigParser(std::stringstream &ss, Server &res, std::string &str);
