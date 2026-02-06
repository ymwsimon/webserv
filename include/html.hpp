/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   html.hpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mayeung <mayeung@student.42london.com>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/12/02 16:35:11 by mayeung           #+#    #+#             */
/*   Updated: 2026/02/06 13:50:45 by mayeung          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once
#include <string>
#include <vector>
#include <map>

const std::string	DOCTYPE = "!DOCTYPE";
const std::string	HTML = "html";
const std::string	HEADTag = "head";
const std::string	TITLE = "title";
const std::string	BODY = "body";

std::string			getStatusBody(int code);
std::string			&genHtmlTagStart(std::string tag, std::string &res);
std::string			genHtmlTagStart(std::string tag);
std::string			&genHtmlTagStart(std::string tag, std::vector<std::pair<std::string, std::string> > &attr, std::string &res);
std::string			genHtmlTagStart(std::string tag, std::vector<std::pair<std::string, std::string> > &attr);
std::string			&genHtmlTagEnd(std::string tag, std::string &res);
std::string			genHtmlTagEnd(std::string tag);
// std::string			&appendHtmlTag(std::string tag, std::string content, std::string &res);
std::string			&appendHtmlTag(std::string tag, std::string &content);
std::string			&appendHtmlTag(std::string tag,
	std::vector<std::pair<std::string, std::string> > &attr, std::string &res);
