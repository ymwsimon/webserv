/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   http.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mayeung <mayeung@student.42london.com>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/12/02 23:48:00 by mayeung           #+#    #+#             */
/*   Updated: 2025/12/03 22:51:02 by mayeung          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../include/http.hpp"

const std::pair<int, std::string>	m[] = {
	std::make_pair(100, "Continue"),
	std::make_pair(101, "Switching Protocols"),
	std::make_pair(102, "Processing"),
	std::make_pair(103, "Early Hints"),
	std::make_pair(200, "OK"),
	std::make_pair(201, "Created"),
	std::make_pair(202, "Accepted"),
	std::make_pair(203, "Non-Authoritative Information"),
	std::make_pair(204, "No Content"),
	std::make_pair(205, "Reset Content"),
	std::make_pair(206, "Partial Content"),
	std::make_pair(207, "Multi-Status"),
	std::make_pair(208, "Already Reported"),
	std::make_pair(226, "IM Used"),
	std::make_pair(300, "Multiple Choices"),
	std::make_pair(301, "Moved Permanently"),
	std::make_pair(302, "Found"),
	std::make_pair(303, "See Other"),
	std::make_pair(304, "Not Modified"),
	std::make_pair(305, "Use Proxy"),
	std::make_pair(306, "unused"),
	std::make_pair(307, "Temporary Redirect"),
	std::make_pair(308, "Permanent Redirect"),
	std::make_pair(400, "Bad Request"),
	std::make_pair(401, "Unauthorized"),
	std::make_pair(402, "Payment Required"),
	std::make_pair(403, "Forbidden"),
	std::make_pair(404, "Not Found"),
	std::make_pair(405, "Method Not Allowed"),
	std::make_pair(406, "Not Acceptable"),
	std::make_pair(407, "Proxy Authentication Required"),
	std::make_pair(408, "Request Timeout"),
	std::make_pair(409, "Conflict"),
	std::make_pair(410, "Gone"),
	std::make_pair(411, "Length Required"),
	std::make_pair(412, "Precondition Failed"),
	std::make_pair(413, "Content Too Large"),
	std::make_pair(414, "URI Too Long"),
	std::make_pair(415, "Unsupported Media Type"),
	std::make_pair(416, "Range Not Satisfiable"),
	std::make_pair(417, "Expectation Failed"),
	std::make_pair(418, "I'm a teapot"),
	std::make_pair(421, "Misdirected Request"),
	std::make_pair(422, "Unprocessable Content"),
	std::make_pair(423, "Locked"),
	std::make_pair(424, "Failed Dependency"),
	std::make_pair(425, "Too Early"),
	std::make_pair(426, "Upgrade Required"),
	std::make_pair(428, "Precondition Required"),
	std::make_pair(429, "Too Many Requests"),
	std::make_pair(431, "Request Header Fields Too Large"),
	std::make_pair(451, "Unavailable For Legal Reasons"),
	std::make_pair(500, "Internal Server Error"),
	std::make_pair(501, "Not Implemented"),
	std::make_pair(502, "Bad Gateway"),
	std::make_pair(503, "Service Unavailable"),
	std::make_pair(504, "Gateway Timeout"),
	std::make_pair(505, "HTTP Version Not Supported"),
	std::make_pair(506, "Variant Also Negotiates"),
	std::make_pair(507, "Insufficient Storage"),
	std::make_pair(508, "Loop Detected"),
	std::make_pair(510, "Not Extended"),
	std::make_pair(511, "Network Authentication Required"),
};

const std::map<int, std::string>	messageTable = std::map<int, std::string>(m, m + sizeof(m) / sizeof(m[0]));

const std::pair<std::string, std::string>	mt[] = {
	std::make_pair("html", "text/html"),
	std::make_pair("htm", "text/html"),
	std::make_pair("css", "text/css"),
	std::make_pair("xml", "text/xml"),
	std::make_pair("gif", "image/gif"),
	std::make_pair("jpeg", "image/jpeg"),
	std::make_pair("jpg", "image/jpeg"),
	std::make_pair("js", "application/javascript"),
	std::make_pair("rss", "application/rss+xml"),
	std::make_pair("txt", "text/plain"),
	std::make_pair("htc", "text/x-component"),
	std::make_pair("avif", "image/avif"),
	std::make_pair("png", "image/png"),
	std::make_pair("svg", "image/svg+xml"),
	std::make_pair("svgz", "image/svg+xml"),
	std::make_pair("tif", "image/tiff"),
	std::make_pair("tiff", "image/tiff"),
	std::make_pair("webp", "image/webp"),
	std::make_pair("ico", "image/x-icon"),
	std::make_pair("jng", "image/x-jng"),
	std::make_pair("bmp", "image/x-ms-bmp"),
	std::make_pair("jar", "application/java-archive"),
	std::make_pair("json", "application/json"),
	std::make_pair("hqx", "application/mac-binhex40"),
	std::make_pair("doc", "application/msword"),
	std::make_pair("pdf", "application/pdf"),
	std::make_pair("ps", "application/postscript"),
	std::make_pair("eps", "application/postscript"),
	std::make_pair("rtf", "application/rtf"),
	std::make_pair("xls", "application/vnd.ms-excel"),
	std::make_pair("ppt", "application/vnd.ms-powerpoint"),
	std::make_pair("pptx", "application/vnd.openxmlformats-officedocument.presentationml.presentation"),
	std::make_pair("xlsx", "application/vnd.openxmlformats-officedocument.spreadsheetml.sheet"),
	std::make_pair("docx", "application/vnd.openxmlformats-officedocument.wordprocessingml.document"),
	std::make_pair("7z", "application/x-7z-compressed"),
	std::make_pair("run", "application/x-makeself"),
	std::make_pair("pl", "application/x-perl"),
	std::make_pair("rar", "application/x-rar-compressed"),
	std::make_pair("rpm", "application/x-redhat-package-manager"),
	std::make_pair("tcl", "application/x-tcl"),
	std::make_pair("xpi", "application/x-xpinstall"),
	std::make_pair("xhtml", "application/xhtml+xml"),
	std::make_pair("zip", "application/zip"),
	std::make_pair("bin", "application/octet-stream"),
	std::make_pair("exe", "application/octet-stream"),
	std::make_pair("dll", "application/octet-stream"),
	std::make_pair("deb", "application/octet-stream"),
	std::make_pair("dmg", "application/octet-stream"),
	std::make_pair("iso", "application/octet-stream"),
	std::make_pair("img", "application/octet-stream"),
	std::make_pair("msi", "application/octet-stream"),
	std::make_pair("msp", "application/octet-stream"),
	std::make_pair("msm", "application/octet-stream"),
	std::make_pair("mid", "audio/midi"),
	std::make_pair("midi", "audio/midi"),
	std::make_pair("kar", "audio/midi"),
	std::make_pair("mp3", "audio/mpeg"),
	std::make_pair("ogg", "audio/ogg"),
	std::make_pair("m4a", "audio/x-m4a"),
	std::make_pair("ra", "audio/x-realaudio"),
	std::make_pair("3gpp", "video/3gpp"),
	std::make_pair("3gp", "video/3gpp"),
	std::make_pair("ts", "video/mp2t"),
	std::make_pair("mp4", "video/mp4"),
	std::make_pair("mpeg", "video/mpeg"),
	std::make_pair("mpg", "video/mpeg"),
	std::make_pair("mov", "video/quicktime"),
	std::make_pair("webm", "video/webm"),
	std::make_pair("flv", "video/x-flv"),
	std::make_pair("m4v", "video/x-m4v"),
	std::make_pair("mng", "video/x-mng"),
	std::make_pair("asx", "video/ms-asf"),
	std::make_pair("asf", "video/ms-asf"),
	std::make_pair("wmv", "video/x-ms-wmv"),
	std::make_pair("avi", "video/x-ms-msvideo"),
};

const std::map<std::string, std::string>	mediaTypeTable =
	std::map<std::string, std::string>(mt, mt + sizeof(mt) / sizeof(mt[0]));

const std::string	&getStatusMessage(int code)
{
	if (messageTable.count(code))
		return messageTable.at(code);
	return messageTable.at(404);
}

const std::string	&getMediaType(std::string mType)
{
	if (mediaTypeTable.count(mType))
		return mediaTypeTable.at(mType);
	return mediaTypeTable.at("bin");
}

std::string	getFullStatusMessage(int code)
{
	if (messageTable.count(code) == 0)
		code = 404;
	return intToString(code) + " " + getStatusMessage(code);
}

std::string	genHttpHeader(std::string fieldName, std::string fieldValue)
{
	return fieldName + ": " + fieldValue + CRLFStr;
}

std::string	genHttpResponseLine(int code)
{
	return defaultHTTPVer + " " + getFullStatusMessage(code) + CRLFStr;
}
