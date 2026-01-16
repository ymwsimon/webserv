/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   http.hpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mayeung <mayeung@student.42london.com>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/12/02 23:46:32 by mayeung           #+#    #+#             */
/*   Updated: 2026/01/13 18:15:16 by mayeung          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once
#include <string>
#include <vector>
#include <map>
#include "utils.hpp"

const std::string	defaultHTTPVer = "HTTP/1.1";
const std::string	CONTENTTYPE = "content-type";
const std::string	CONTENTLENGTH = "content-length";

enum e_HTTPStatusCode
{
	HTTP_OK = 200,
	BAD_REQUEST = 400,
	FORBIDDEN = 403,
	NOT_FOUND = 404,
	INTERNAL_ERROR = 500,
};

const std::string	&getStatusMessage(int code);
const std::string	&getMediaType(std::string mType);
std::string			getFullStatusMessage(int code);
std::string			genHttpHeader(std::string fieldName, std::string fieldValue);
std::string			genHttpResponseLine(int code);
std::string			genHtmlPage(std::string title, std::string content);
std::string			genHttpResponse(int code);
std::string			genHttpResponse(int code, const std::string &content);
std::string			genHttpResponse(int code, const std::string mediaType, const std::string &content);
