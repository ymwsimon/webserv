/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   http.hpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mayeung <mayeung@student.42london.com>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/12/02 23:46:32 by mayeung           #+#    #+#             */
/*   Updated: 2026/01/29 11:29:15 by mayeung          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once
#include <string>
#include <vector>
#include <map>
#include "utils.hpp"

const unsigned char EOT = 4;
const std::string	defaultHTTPVer = "HTTP/1.1";
const std::string	CONTENT_TYPE = "content-type";
const std::string	CONTENT_LENGTH = "content-length";
const std::string	TRANSFER_ENDCODING = "transfer-encoding";
const std::string	CHUNKED = "chunked";

enum e_HTTPStatusCode
{
	HTTP_OK = 200,
	NO_CONTENT = 204,
	BAD_REQUEST = 400,
	FORBIDDEN = 403,
	NOT_FOUND = 404,
	NOT_ALLOWED = 405,
	CONFLICT = 409,
	Length_Required = 411,
	Content_Too_Large = 413,
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
std::string			genHttpResponse(int code, std::string mediaType, const std::string &content);
std::string			genHttpResponse(int code, const std::string &content,
	std::map<std::string, std::string> headers);
std::string			genHttpResponse(int code, std::map<std::string, std::string> headers);
