#include "RequestParser.hpp"
#include "enums.hpp"
#include "StringManip.hpp"
#include <iostream>
#include <cstdlib>
#include <cctype>
#include "utils.hpp"
#include "PostParser.hpp"
#include "Executor.hpp"

RequestParser ::RequestParser() {}

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
			return ;

		if (size == 0)
		{
			if (request.requestSize() < pos + 4)
				return;
			if (request.getCharFromRequest(pos + 2) != '\r'
				|| request.getCharFromRequest(pos + 3) != '\n')
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
			std::cout << "salam";
			request.setResponseCode(HP_PAYLOAD_TOO_LARGE);
			return;
		}
		pos = findCrlfPos(request);
	}
}

/* body fixed length parsing  */
void RequestParser::read_body_fixed(HttpRequest &request)
{
    Body	&body = request.getBody();
    size_t	to_read = body.getToRead(); // this should be set in header if there's one .
    size_t	available_data = request.requestSize();

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

		std::cout << "salam2 " << content_length << " " << max;
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

void RequestParser::read_header(HttpRequest &request, HttpResponse &response)
{

    size_t pos;
    pos = find2CrlfPos(request);
    if (pos == std::string::npos)
        return;
    if (pos > BUFFER_SIZE)
    {
		std::cout << "salam3";
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
	request.parseCookies();

	// after reading headers, seting the body type
	//if the body is empty, set the status to finished
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
	if (request.getType() == POST )
	{
		// condition: for mulipartformdata
		if (request.getCGIType() == NO_CGI)
		{
			std::string contentType = request.getHeader("content-type");
			if (contentType.find("multipart/form-data") != std::string::npos)
			{
				std::string boundary = PostParser::extractBoundary(contentType);
				if (boundary.empty() || boundary.size() > 70)
				{
					response.setStatus(HP_BAD_REQUEST);
					response.setState(RESPONSE_FINISHED);
					return;
				}
				request.setBoundary("--" + boundary);
				return ;
			}
		}
		PostParser::creatFile(request, response); // this creat the file for : CGI | upload file that is not multipart/form-data
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

void RequestParser::reading_request_line(HttpRequest &request, HttpResponse &response)
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
			if (token.size() > BUFFER_SIZE - 18) //18 is the size of largest method (DELETE) +  (space) + 2 (HTTP/1.1) + 2 (CRLF)
			{
				request.setResponseCode(HP_URI_TOO_LONG);
				return;
			}
			size_t query_pos = token.find('?');
            std::string qStr;
            if (query_pos != std::string::npos) {
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
	request.setStatus(READ_HEADER);
	request.eraseFromRequest(0, pos + 2);

	Executor::setLocation(request);

	if (request.getLocation() == NULL)
	{
		response.setStatus(HP_NOT_FOUND);
		response.setState(RESPONSE_FINISHED);
		request.setStatus(ERROR);
		return ;
	}

	if (!Executor::isAllowedMethod(request))
	{
		response.setStatus(HP_METHOD_NOT_ALLOWED);
		response.setState(RESPONSE_FINISHED);
		request.setStatus(ERROR);
		return ;
	}
	if (request.getType() == POST) {
		if (request.getLocation()->upload_store.empty()) {
			response.setStatus(HP_FORBIDDEN);
			response.setState(RESPONSE_FINISHED);
			request.setStatus(ERROR);
		}
	}
}

/* ************************************************************************** */
/*                                                                            */
/*                           CREATE_REQUEST 								  */
/*                                                                            */
/* ************************************************************************** */

void RequestParser::create_request(HttpRequest &request, HttpResponse &response)
{
	if (request.getStatus() == READING_REQUEST_LINE)
        reading_request_line(request, response);
	if (request.getStatus() == READ_HEADER)
        read_header(request, response); // file created.
    if (request.getStatus() == READ_BODY)
	{
		read_body(request);
		// if (request.getStatus() == ERROR)
		// {
			// if (response.getStatus() == 0)
			// {
			// 	response.setStatus(request.getResponseCode());
			// 	response.setState(RESPONSE_FINISHED);
			// }
			// return;
		// }
		if (request.getType() == POST && request.getCGIType() != NO_CGI)
		{
			PostParser::executeCGI(request);
		}
		if (request.getType() == POST && request.getCGIType() == NO_CGI)
		{
			PostParser::executeUpload(request, response);
		}
	}
	if (request.getStatus() == FINISHED)
        return;

	if (request.getStatus() == ERROR)
	{
		if (response.getStatus() == 0)
		{
			response.setStatus(request.getResponseCode());
			response.setState(RESPONSE_FINISHED);
		}
		return ;
	}
}