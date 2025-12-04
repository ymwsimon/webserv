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

const std::string	DOCTYPE = "!DOCTYPE";
const std::string	HTML = "html";
const std::string	HEAD = "head";
const std::string	TITLE = "title";
const std::string	BODY = "body";
const std::string	defaultHTTPVer = "HTTP/1.1";

const std::string	&getStatusMessage(int code);
const std::string	&getMediaType(std::string mType);
std::string			getFullStatusMessage(int code);
std::string			genHttpHeader(std::string fieldName, std::string fieldValue);
std::string			genHttpResponseLine(int code);