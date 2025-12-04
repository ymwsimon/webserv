/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   html.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mayeung <mayeung@student.42london.com>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/12/02 16:41:15 by mayeung           #+#    #+#             */
/*   Updated: 2025/12/02 23:47:56 by mayeung          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../include/html.hpp"

std::string	getStatusBody(int code)
{
	std::string	res;
	(void)code;
	return "";
}

std::string	genHtmlTagStart(std::string tag)
{
	return "<" + tag + ">";
}

std::string	genHtmlTagStart(std::string tag, std::vector<std::pair<std::string, std::string> > attr)
{
	std::string	res = " ";

	for (size_t i = 0; i < attr.size(); ++i)
	{
		res.append(attr[i].first);
		res.append("=\"");
		res.append(attr[i].second);
		res.append("\"");
	}
	if (attr.size())
		return "<" + tag + res + ">";
	return "<" + tag + ">";
}

std::string	genHtmlTagEnd(std::string tag)
{
	return "</" + tag + ">";
}

std::string	&appendHtmlTag(std::string tag, std::string &content)
{
	content.insert(0, genHtmlTagStart(tag));
	content.append(genHtmlTagEnd(tag));
	return content;
}

std::string	&appendHtmlTag(std::string tag,
	std::vector<std::pair<std::string, std::string> > attr, std::string &content)
{
	content.insert(0, genHtmlTagStart(tag, attr));
	content.append(genHtmlTagEnd(tag));
	return content;
}

std::string	genHtmlPage(std::string title, std::string content)
{
	std::string	res;

	appendHtmlTag(TITLE, title);
	appendHtmlTag(HEAD, title);
	appendHtmlTag(BODY, content);
	res = title + content;
	appendHtmlTag(HTML, res);
	res = genHtmlTagStart(DOCTYPE + " " + HTML) + res;
	return res;
}
