#include "RequestParser.hpp"
#include "enums.hpp"
#include "StringManip.hpp"
#include <iostream>
#include <cstdlib>
#include <cctype>

RequestParser ::RequestParser() {}
/* ************************************************************************** */
/*                                                                            */
/*                           UTILS FUNCTIONS 					  			  */
/*                                                                            */
/* ************************************************************************** */

size_t RequestParser::findEndOfHeader(HttpRequest &request)
{

    size_t pos = 0;
    while (pos < request.getRequest().size() - 3)
    {
        if (request.getRequest().at(pos) == '\r' && request.getRequest().at(pos + 1) == '\n' &&
            request.getRequest().at(pos + 2) == '\r' && request.getRequest().at(pos + 3) == '\n')
            return (pos);
        pos++;
    }
    return (std::string::npos);
}

size_t RequestParser::findCrlfPos(HttpRequest &request)
{
    size_t pos = 0;
    while (pos < request.getRequest().size() - 1)
    {
        if (request.getRequest().at(pos) == '\r' && request.getRequest().at(pos + 1) == '\n')
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

bool RequestParser::get_chunked(HttpRequest &request, int size)
{
    if (!(request.getCharFromRequest(size) == '\r' && request.getCharFromRequest(size + 1) == '\n'))
        return (false);
    request.getBody().appendChunkToBody(request.getChunk(0, size));
    request.eraseFromRequest(0, size + 2);
    return (true);
}

void RequestParser::read_body_chunked(HttpRequest &request)
{
    size_t pos = findCrlfPos(request);
    if (pos == std::string::npos)
        return;

    std::string line = request.extractString(0, pos);
    size_t i = 0;
    while (i < line.size() && std::isxdigit((line[i])))
        i++;
    std::string hex_part = line.substr(0, i);
    if (hex_part.empty())
    {
        request.setResponseCode(400, "Bad Request");
        return;
    }
    int size = hex_to_num(hex_part);
    if (size < 0)
    {
        request.setResponseCode(400, "Bad Request");
        return;
    }
    if (size == 0)
    {
        request.eraseFromRequest(0, pos + 2);
        request.setStatus((status)FINISHED);
        return;
    }
    if (request.requestSize() < pos + 2 + (size) + 2)
        return;
    request.eraseFromRequest(0, pos + 2);
    if (get_chunked(request, size) == false)
    {
        request.setResponseCode(400, "Bad Request");
        return;
    }
}

void RequestParser::read_body_fixed(HttpRequest &request)
{
    Body	&body = request.getBody();
    size_t	to_read = body.getToRead();
    size_t	available_data = request.requestSize();

    if (to_read >= available_data)
    {
        body.appendChunkToBody(request.getChunk(0, available_data));
        body.decrementToRead(available_data);
	    request.clear();
    }
    else
    {
        body.appendChunkToBody(request.getChunk(0, to_read));
        request.eraseFromRequest(0, to_read);
        body.setToRead(0);
    }
    if (body.getToRead() == 0)
        request.setStatus((status)FINISHED);
}

bool RequestParser::setBufferFixed(HttpRequest &request, Body *body)
{
	int content_length = stringToNumber(request.getHeader(FIXED_LENGTH_HEADER));
    if (content_length <= -1)
	{
		request.setResponseCode(400, "Bad Request");
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
    pos = findEndOfHeader(request);
    if (pos == std::string::npos)
        return;
    std::string headers_string = request.extractString(0, pos);
    request.eraseFromRequest(0, pos + 4);
    std::string line;

    while ((line = StringManip::get_token(headers_string, '\n')).empty() == false)
    {
        std::pair<std::string, std::string> header = StringManip::split_two(line, ':');
		
        if (header.first.empty() == true || StringManip::strtrim(header.first) != header.first)
        {
			request.setResponseCode(400, "Bad Request");
            return;
        }
        request.addHeader(StringManip::toLowerCase(header.first), header.second);
        headers_string.erase(0, line.length() + 1);
    }
	// after reading headers, seting the body type 
	//if the body is empty, set the status to finished
	if (request.getHttpVersion() == HTTP_1_1 && request.getHeader("host").empty())
	{
		request.setResponseCode(400, "Bad Request");
		return;
	}
	if (request.getBody().discoverReadingType(request) == EMPTY)
	{
		request.setStatus(FINISHED);
		return;
	}
    request.setStatus(READ_BODY);
}

/* ************************************************************************** */
/*                                                                            */
/*                           REQUEST_LINE parsing 						  */
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
		request.setResponseCode(400, "Bad Request");
		return false;
	}
    else
	{
		request.setResponseCode(405, "Method Not Allowed");
		return false;
	}
	return true;
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
			if (token.size() > BUFFER_SIZE - 18) //18 is the size of largest method (DELETE) +  (space) + 2 (HTTP/1.1) + 2 (CRLF)
			{
				request.setResponseCode(414, "URI Too Long");
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
            request.setResponseCode(400, "Bad Request");
            return;
        }
        line = StringManip::strtrim(line);
	}
	if (token_numbers != 3)
	{
		request.setResponseCode(400, "Bad Request");
		return;
	}
	request.setStatus(READ_HEADER);
	request.eraseFromRequest(0, pos + 2);
}

/* ************************************************************************** */
/*                                                                            */
/*                           CREATE_REQUEST 								  */
/*                                                                            */
/* ************************************************************************** */

//changing the order of the function (request-line -> header -> body)
void RequestParser::create_request(HttpRequest &request)
{
	if (request.getStatus() == FINISHED)
        return;
    else if (request.getStatus() == READING_REQUEST_LINE)
        reading_request_line(request);
    else if (request.getStatus() == READ_HEADER)
        read_header(request);
    else if (request.getStatus() == READ_BODY)
        read_body(request);
	else // ERROR
		return;
}