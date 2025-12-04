/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   http.hpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mayeung <mayeung@student.42london.com>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/12/02 23:46:32 by mayeung           #+#    #+#             */
/*   Updated: 2025/12/03 22:51:30 by mayeung          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once
#include <string>
#include <vector>
#include <map>
#include "utils.hpp"

const std::string	defaultHTTPVer = "HTTP/1.1";
const std::string	CONTENTTYPE = "Content-Type";
const std::string	CONTENTLENGTH = "Content-Length";

const std::string	&getStatusMessage(int code);
const std::string	&getMediaType(std::string mType);
std::string			getFullStatusMessage(int code);
std::string			genHttpHeader(std::string fieldName, std::string fieldValue);
std::string			genHttpResponseLine(int code);
std::string			genHtmlPage(std::string title, std::string content);
std::string			genHttpResponse(int code);
std::string			genHttpResponse(int code, const std::string &content);
std::string			genHttpResponse(int code, const std::string mediaType, const std::string &content);
