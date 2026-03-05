#include "RequestParser.hpp"
#include "enums.hpp"
#include "StringManip.hpp"
#include "utils.hpp"
#include "PostParser.hpp"
#include "Executor.hpp"
#include <iostream>
#include <cstdlib>
#include <cctype>
#include <algorithm>
#include "Session.hpp"

RequestParser ::RequestParser() {}

/* ************************************************************************** */
/*                                                                            */
/*                           COOKIES					              	  	  */
/*                                                                            */
/* ************************************************************************** */

void RequestParser::parseOrCreateCookie(HttpRequest &request, HttpResponse &response)
{
	std::cout << "------------------ parsing cookies --------------------------------" << std::endl;
	if (request.getConfig()->getSessionManager() == NULL)
		request.getConfig()->setSessionManager(new SessionManager());

	if (request.getHeader("cookie").empty())
	{
		std::cout << "creating session" << std::endl;
		std::string session_id = request.getConfig()->getSessionManager()->createSession(TIME_EXPIRATION)->getSessionId();
		response.AddHeader("Set-Cookie", "session_id=" + session_id + "; Path=/; HttpOnly\r\n");
		response.setSession(request.getConfig()->getSessionManager()->getSession(session_id));
	}
	else
	{
		request.parseCookies();
		std::string session_id = request.getCookie("session_id");
		Session *session = request.getConfig()->getSessionManager()->getSession(session_id);
		if (session == NULL || session->isExpired())
		{
			request.getConfig()->getSessionManager()->deleteSession(session_id);
			std::string new_session_id = request.getConfig()->getSessionManager()->createSession(TIME_EXPIRATION)->getSessionId();
			response.AddHeader("Set-Cookie", "session_id=" + new_session_id + "; Path=/; HttpOnly\r\n");
			if (session != NULL)
				request.getConfig()->getSessionManager()->deleteSession(session_id);
			response.setSession(request.getConfig()->getSessionManager()->getSession(new_session_id));
		}
		else
		{
			response.AddHeader("Set-Cookie", "session_id=" + session_id + "; Path=/; HttpOnly\r\n");
			response.setSession(session);
		}
	}
}

void RequestParser::responseGeneration(HttpRequest &request, HttpResponse &response)
{
	response.setStatus(HP_FOUND);
	response.AddHeader("Location", "/cookies\r\n");
	response.AddHeader("Connection", "closed\r\n");
	response.AddHeader("server", "TestServer/1.1\r\n");
	response.AddHeader("content-length", "0\r\n");
	response.AddHeader("set-cookie", "session_id=" + response.getSession()->getSessionId() + "; Path=/; HttpOnly\r\n");
}
void RequestParser::PostCookiesHandler(HttpRequest &request, HttpResponse &response)
{
	std ::cout << "------------------ handling cookies in POST --------------------------------" << std::endl;
	std::string body_str(request.getBody().getBody().begin(), request.getBody().getBody().end());
	std::string username;
	size_t pos = body_str.find("username=");
	if (pos != std::string::npos)
	{
		username = body_str.substr(pos + 9);
		size_t amp_pos = username.find('&');
		if (amp_pos != std::string::npos)
		{
			username = username.substr(0, amp_pos);
		}
	}
	if (username.empty())
	{
		response.setStatus(HP_BAD_REQUEST);
		response.setState(RESPONSE_FINISHED);
		return;
	}
	response.getSession()->setData("username", username);
	std::cout << "Username set in session: " << username << std::endl;
	responseGeneration(request, response);
	response.setState(RESPONSE_FINISHED);
	request.setStatus(FINISHED);
}

/* ************************************************************************** */
/*                                                                            */
/*                           UTILS FUNCTIONS 					  			  */
/*                                                                            */
/* ************************************************************************** */

size_t RequestParser::find2CrlfPos(HttpRequest &request)
{
	size_t len = request.requestSize();
	size_t pos = 0;
	while (pos + 3 < len)
	{
		if (request.getCharFromRequest(pos) == '\r' && request.getCharFromRequest(pos + 1) == '\n' &&
			request.getCharFromRequest(pos + 2) == '\r' && request.getCharFromRequest(pos + 3) == '\n')
			return (pos);
		pos++;
	}
	return (std::string::npos);
}

size_t RequestParser::findCrlfPos(HttpRequest &request)
{
	size_t len = request.requestSize();
	size_t pos = 0;
	while (pos + 1 < len)
	{
		if (request.getCharFromRequest(pos) == '\r' && request.getCharFromRequest(pos + 1) == '\n')
			return (pos);
		pos++;
	}
	return (std::string::npos);
}

/* ************************************************************************** */
/*                                                                            */
/*                           BODY parsing 								  */
/*                                                                            */
/* ************************************************************************** */

/* body chunked parsing  */
bool RequestParser::get_chunked(HttpRequest &request, long long size)
{
	if (!(request.getCharFromRequest(size) == '\r' && request.getCharFromRequest(size + 1) == '\n'))
		return (false);
	request.getBody().appendChunkToBody(request.getChunk(0, size));
	request.eraseFromRequest(0, size + 2);
	return (true);
}

long long RequestParser::get_chunked_size(HttpRequest &request, const size_t &pos)
{
	std::string line = request.extractString(0, pos);
	size_t i = 0;
	while (i < line.size() && std::isxdigit((line[i])))
		i++;
	std::string hex_part = line.substr(0, i);
	if (hex_part.empty())
	{
		request.setResponseCode(HP_BAD_REQUEST);
		return (-1);
	}
	long long size = hex_to_num(hex_part);
	if (size < 0)
	{
		request.setResponseCode(HP_BAD_REQUEST);
		return (-1);
	}
	return (size);
}

void RequestParser::read_body_chunked(HttpRequest &request)
{
	size_t pos = findCrlfPos(request);

	while (pos != std::string::npos)
	{
		long long size = get_chunked_size(request, pos);
		if (size < 0)
			return;

		if (size == 0)
		{
			if (request.requestSize() < pos + 4)
				return;
			if (request.getCharFromRequest(pos + 2) != '\r' || request.getCharFromRequest(pos + 3) != '\n')
			{
				request.setResponseCode(HP_BAD_REQUEST);
				return;
			}
			request.eraseFromRequest(0, pos + 4);
			request.setStatus((status)FINISHED);
			return;
		}

		if (request.requestSize() < pos + 2 + (size) + 2)
			return;

		request.eraseFromRequest(0, pos + 2);
		if (get_chunked(request, size) == false)
		{
			request.setResponseCode(HP_BAD_REQUEST);
			return;
		}
		size_t max = request.getLocation()->clientMaxSizeBody;
		if (max > 0 && request.getBody().bodySize() > max)
		{
			request.setResponseCode(HP_PAYLOAD_TOO_LARGE);
			return;
		}
		pos = findCrlfPos(request);
	}
}

/* body fixed length parsing  */
void RequestParser::read_body_fixed(HttpRequest &request)
{
	Body &body = request.getBody();
	size_t to_read = body.getToRead(); // this should be set in header if there's one .
	size_t available_data = request.requestSize();

	if (to_read >= available_data)
	{
		body.appendChunkToBody(request.getChunk(0, available_data));
		body.decrementToRead(available_data);
		std::cout << "to read:" << body.getToRead() << std::endl;
		request.clear();
	}
	else
	{
		body.appendChunkToBody(request.getChunk(0, to_read));
		request.eraseFromRequest(0, to_read); // here we can do simply request.clear() , since we can handle keep-alive , those leftovers bytes matters .
		body.setToRead(0);
	}
	if (body.getToRead() == 0)
	{
		request.setStatus((status)FINISHED);
	}
}

bool RequestParser::setBufferFixed(HttpRequest &request, Body *body)
{
	long long content_length = stringToNumber(request.getHeader(FIXED_LENGTH_HEADER));
	if (content_length <= -1)
	{
		request.setResponseCode(HP_BAD_REQUEST);
		body->setType(EMPTY);
		return (false);
	}
	size_t max = request.getLocation()->clientMaxSizeBody;
	if (max > 0 && (size_t)content_length > max)
	{
		request.setResponseCode(HP_PAYLOAD_TOO_LARGE);
		body->setType(EMPTY);
		return (false);
	}
	body->setToRead((size_t)content_length);
	return (true);
}

void RequestParser::read_body(HttpRequest &request)
{
	Body &body = request.getBody();

	if (body.getType() == FIXED_LENGTH)
	{
		if (body.getToRead() == 0)
		{
			if (!setBufferFixed(request, &body))
				return;
		}
		read_body_fixed(request);
	}
	if (body.getType() == CHUNKED)
		read_body_chunked(request);
}

/* ************************************************************************** */
/*                                                                            */
/*                           HEADER parsing 								  */
/*                                                                            */
/* ************************************************************************** */

void RequestParser::read_header(HttpRequest &request)
{

	size_t pos;
	pos = find2CrlfPos(request);
	if (pos == std::string::npos)
		return;
	if (pos > BUFFER_SIZE)
	{
		request.setResponseCode(HP_REQUEST_HEADER_TOO_LARGE);
		return;
	}
	std::string headers_string = request.extractString(0, pos);
	request.eraseFromRequest(0, pos + 4);
	std::string line;

	while ((line = StringManip::get_token(headers_string, '\n')).empty() == false)
	{
		std::pair<std::string, std::string> header = StringManip::split_two(line, ':');

		if (header.first.empty() == true || StringManip::strtrim(header.first) != header.first)
		{
			request.setResponseCode(HP_BAD_REQUEST);
			return;
		}
		request.addHeader(StringManip::toLowerCase(header.first), header.second);
		headers_string.erase(0, line.length() + 1);
	}

	parseOrCreateCookie(request, response);
	// after reading headers, seting the body type
	// if the body is empty, set the status to finished
	if (request.getHttpVersion() == HTTP_1_1 && request.getHeader("host").empty())
	{
		request.setResponseCode(HP_BAD_REQUEST);
		return;
	}

	if (request.getBody().discoverReadingType(request) == EMPTY)
	{
		request.setStatus(FINISHED);
		return;
	}

	request.setStatus(READ_BODY);
	/*after reading the headers we creatFile if there is body read and fileupload*/
	if (request.getType() == POST)
	{

		// condition: for mulipartformdata
		if (request.getUri() == "/cookies")
		{
			return;
		}
		if (request.getCGIType() == NO_CGI)
		{
			std::string contentType = request.getHeader("content-type");
			if (contentType.find("multipart/form-data") != std::string::npos)
			{
				std::string boundary = PostParser::extractBoundary(contentType);
				if (boundary.empty() || boundary.size() > 70)
				{
					request.setResponseCode(HP_BAD_REQUEST);
					return;
				}
				request.setBoundary("--" + boundary);
				return;
			}
		}
		PostParser::creatFile(request); // this creat the file for : CGI | upload file that is not multipart/form-data
	}
}

/* ************************************************************************** */
/*                                                                            */
/*                           REQUEST_LINE parsing 						  	  */
/*                                                                            */
/* ************************************************************************** */

bool RequestParser::set_request_type(HttpRequest &request, std::string token)
{

	if (token.compare("GET") == 0)
		request.setType(GET);
	else if (token.compare("POST") == 0)
		request.setType(POST);
	else if (token.compare("DELETE") == 0)
		request.setType(DELETE);
	else if (!StringManip::isAllUppercase(token))
	{
		request.setResponseCode(HP_BAD_REQUEST);
		return false;
	}
	else
	{
		request.setResponseCode(HP_NOT_IMPLEMENTED);
		return false;
	}
	return true;
}

void RequestParser::checkLocationRules(HttpRequest &request)
{
	if (request.getLocation() == NULL)
	{
		request.setResponseCode(HP_NOT_FOUND);
		return;
	}

	if (!Executor::isAllowedMethod(request))
	{
		request.setResponseCode(HP_METHOD_NOT_ALLOWED);
		return;
	}

	if (request.getCGIType() != NO_CGI)
	{ // sheck if extension allowed
		std::vector<std::string> extensions = request.getLocation()->cgi_extension;
		CGIType t = request.getCGIType();
		std::string req_extention;
		if (t == PHP_CGI)
		{
			req_extention = ".php";
		}
		else if (t == PYTHON_CGI)
		{
			req_extention = ".py";
		}
		else
		{
			req_extention = ".sh";
		}
		if (std::find(extensions.begin(), extensions.end(), req_extention) == extensions.end())
		{
			request.setResponseCode(HP_FORBIDDEN);
			return;
		}
	}

	if (request.getType() == POST)
	{
		if (request.getLocation()->upload_store.empty())
		{
			request.setResponseCode(HP_FORBIDDEN);
			return;
		}
	}
}

void RequestParser::reading_request_line(HttpRequest &request)
{
	int token_numbers = 0;

	size_t pos = findCrlfPos(request);
	if (pos == std::string::npos)
		return;

	std::string line = request.extractString(0, pos);
	std::string token;
	while ((token = StringManip::get_token(line, ' ')).empty() == false)
	{
		line.erase(0, token.length());
		token_numbers++;

		if (token_numbers == 1)
		{
			if (!set_request_type(request, token))
				return;
		}

		else if (token_numbers == 2)
		{
			if (token.size() > BUFFER_SIZE - 18) // 18 is the size of largest method (DELETE) +  (space) + 2 (HTTP/1.1) + 2 (CRLF)
			{
				request.setResponseCode(HP_URI_TOO_LONG);
				return;
			}
			size_t query_pos = token.find('?');
			std::string qStr;
			if (query_pos != std::string::npos)
			{
				qStr = token.substr(query_pos + 1, token.length() - query_pos);
				request.setQuery(qStr);
				token.erase(query_pos, token.length() - query_pos);
			}
			if (!request.setUri(token))
				return;
		}

		else if (token_numbers == 3)
		{
			if (!request.setHttpVersion(token))
				return;
		}

		else if (token_numbers > 3)
		{
			request.setResponseCode(HP_BAD_REQUEST);
			return;
		}
		line = StringManip::strtrim(line);
	}
	if (token_numbers != 3)
	{
		request.setResponseCode(HP_BAD_REQUEST);
		return;
	}
	request.eraseFromRequest(0, pos + 2);
	request.setStatus(READ_HEADER);
	if (request.getType() == POST && request.getUri() == "/cookies")
		return;
	Executor::setLocation(request);
	checkLocationRules(request);
}

/* ************************************************************************** */
/*                                                                            */
/*                           CREATE_REQUEST 								  */
/*                                                                            */
/* ************************************************************************** */

void RequestParser::create_request(HttpRequest &request, HttpResponse &response)
{
	if (request.getStatus() == READING_REQUEST_LINE)
		reading_request_line(request);
	if (request.getStatus() == READ_HEADER)
		read_header(request);
	if (request.getStatus() == READ_BODY)
	{
		read_body(request);
		if (request.getUri() == "/cookies" && request.getType() == POST)
		{
			PostCookiesHandler(request, response);
			return;
		}
		if (request.getStatus() != ERROR && request.getType() == POST)
		{
			if (request.getCGIType() == NO_CGI)
				PostParser::executeUpload(request);
			else
				PostParser::executeCGI(request);
		}
	}
	if (request.getStatus() == ERROR || request.getResponseCode() == HP_CREATED)
		response.setStatus(request.getResponseCode());
}