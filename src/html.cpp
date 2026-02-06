/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   html.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mayeung <mayeung@student.42london.com>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/12/02 16:41:15 by mayeung           #+#    #+#             */
/*   Updated: 2026/02/06 15:44:37 by mayeung          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../include/html.hpp"

std::string	getStatusBody(int code)
{
	std::string	res;
	(void)code;
	return "";
}

std::string	&genHtmlTagStart(std::string tag, std::string &res)
{
	res.reserve(res.size() + tag.size() + 2);
	res.insert(res.begin(), '>');
	res.insert(0, tag);
	res.insert(res.begin(), '<');
	return res;
}

std::string	genHtmlTagStart(std::string tag)
{
	tag.insert(tag.begin(), '<');
	tag.insert(tag.end(), '>');
	return tag;
}

std::string	&genHtmlTagStart(std::string tag, std::vector<std::pair<std::string, std::string> > &attr, std::string &res)
{
	std::string	fullTag = "<";

	fullTag.append(tag);
	for (size_t i = 0; i < attr.size(); ++i)
	{
		fullTag.append(" ");
		fullTag.append(attr[i].first);
		fullTag.append("=\"");
		fullTag.append(attr[i].second);
		fullTag.append("\"");
	}
	fullTag.append(">");
	res.insert(0, fullTag);
	return res;
}

std::string	genHtmlTagStart(std::string tag, std::vector<std::pair<std::string, std::string> > &attr)
{
	std::string	res;

	for (size_t i = 0; i < attr.size(); ++i)
	{
		res.append(" ");
		res.append(attr[i].first);
		res.append("=\"");
		res.append(attr[i].second);
		res.append("\"");
	}
	return "<" + tag + res + ">";
}

std::string	&genHtmlTagEnd(std::string tag, std::string &res)
{
	res.reserve(res.size() + tag.size() + 3);
	res.append("</");
	res.append(tag);
	res.insert(res.end(), '>');
	return res;
}

std::string	genHtmlTagEnd(std::string tag)
{
	return "</" + tag + ">";
}

std::string	&appendHtmlTag(std::string tag, std::string &content)
{
	genHtmlTagStart(tag, content);
	genHtmlTagEnd(tag, content);
	return content;
}

std::string	&appendHtmlTag(std::string tag,
	std::vector<std::pair<std::string, std::string> > &attr, std::string &res)
{
	genHtmlTagStart(tag, attr, res);
	genHtmlTagEnd(tag, res);
	return res;
}
