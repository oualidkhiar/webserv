#include "RequestParser.hpp"
#include "enums.hpp"
#include "StringManip.hpp"
#include <iostream>
#include <cstdlib>

RequestParser ::RequestParser() {}

bool RequestParser::get_chunked(std::string &chunked, std::string &request_string, int size)
{
    chunked = request_string.substr(0, size);
    if (request_string[size] == '\n')
        return (true);
    return (false);
}

bool RequestParser::setBufferFixed(HttpRequest &request, Body *body)
{
    std::string &request_string = request.getRequest();
    int content_length = stringToNumber(request.getHeader(FIXED_LENGTH_HEADER));
    if (content_length <= -1)
        exit_error("RequestParser::setBufferFixed ERROR content_length Bad fornat\n");
    body->setToRead((size_t)content_length);
    return (true);
}

void RequestParser::read_body_fixed(HttpRequest &request)
{
    // To Implement Later   
}
void RequestParser::read_body_chunked(HttpRequest &request)
{
    Body *body = request.getBody();
    size_t pos;
    int size;
    std::string chunk;
    std::string request_string = request.getRequest();
    pos = request_string.find('\n');
    size = hex_to_num(request_string);
    if (size <= -1)
        exit_error("read_body_chunked ERROR:: size <= -1 \n");
    request_string.erase(0, pos + 1);
    if (get_chunked(chunk, request_string, size) == false)
        exit_error("read_body_chunked ERROR:: Chunked[size]!= 'new_line' \n");
    body->setBody(body->getBody() + '\n' + chunk);
    request_string.erase(0, size + 1);
}

void RequestParser::read_body(HttpRequest &request)
{
    Body *body = request.getBody();
    if (body == NULL)
    {
        body = new Body();
        body->discoverReadingType(request);
        request.setBody(body);
        if (body->getType() == FIXED_LENGTH)
            setBufferFixed(request, body);
    }
    if (body->getType() == CHUNKED)
        read_body_chunked(request);
    else
        read_body_fixed(request);
}
void RequestParser::reading_request_line(HttpRequest &request)
{
    int token_numbers = 0;

    std::string &request_string = request.getRequest();
    size_t pos = request_string.find("\n");
    if (pos == std::string::npos)
        return;
    std::string line = request_string.substr(0, pos);
    std::string token;
    while ((token = StringManip::get_token(line, ' ')).empty() == false)
    {
        // std::cout << "reading_request_line token = " << token << std::endl;
        // std::cout << "reading_request_line line = " << line << std::endl;
        line.erase(0, token.length());

        token_numbers++;
        if (token_numbers == 1)
            set_request_type(request, token);
        else if (token_numbers == 2)
            request.setUri(token);
        // else if (token_numbers = == 3) SET HTTP VERSION IF NEEDED , ELSE CHECK AND PASS
        //             request.setVersion();
        else if (token_numbers > 3)
        {
            request.setResponseCode(400);
            request.setStatus(ERROR);
            std::cout << "reading_request_line::ERROR ARGS MORE THAN EXEPECTED" << std::endl;
            return;
        }
        line = StringManip::strtrim(line);
        //   std::cout << "reading_request_line line = " << line << std::endl;
    }
    if (token_numbers == 3)
        request.setStatus(READ_HEADER);
    request_string.erase(0, pos + 1);
    std::cout << "request[0] = " << request.getRequest().at(0) << std::endl;
    // std::cout << "-------------------AFTER READINF REQEST LINE-------------------------" << token_numbers << std::endl;
    // std::cout << request.getRequest() << std::endl;
}

void RequestParser::read_header(HttpRequest &request)
{
    std::cout << "-----------------------READING HEADER--------------------------" << std::endl;
    std::string &request_string = request.getRequest();
    size_t pos;
    pos = request_string.find("\r\n\r\n");
    if (pos == std::string::npos)
        return;
    std::string headers_string = request_string.substr(0, pos);
    std::cout << headers_string << std::endl;
    std::string line;
    while ((line = StringManip::get_token(headers_string, '\n')).empty() == false)
    {
        std::cout << "---LINE =" << line << std::endl;
        std::pair<std::string, std::string> header = StringManip::split_two(line, ':');
        if (header.second.empty() == true || header.first.empty() == true)
        {
            request.setResponseCode(400);
            request.setStatus(ERROR);
            std::cout << "read_header::ERROR HEADER NOT VALID" << std::endl;
            return;
        }
        request.addHeader(header.first, header.second);
        headers_string.erase(0, line.length() + 1);
    }
    if (request.getType() == GET || request.getType() == DELETE)
        request.setStatus(FINISHED);
    else
        request.setStatus(READ_BODY);
}
void RequestParser::create_request(HttpRequest &request)
{
    if (request.getStatus() == FINISHED)
        return;
    else if (request.getStatus() == READ_HEADER)
        read_header(request);
    else if (request.getStatus() == READ_BODY)
        read_body(request);
    else if (request.getStatus() == READING_REQUEST_LINE)
    {
        reading_request_line(request);
    }
    else
        read_body(request);
}

void RequestParser::set_request_type(HttpRequest &request, std::string token)
{
    std::cout << "set_request_type token=" << token << std::endl;
    if (token.compare("GET") == 0)
        request.setType(GET);
    else if (token.compare("POST") == 0)
        request.setType(POST);
    else if (token.compare("DELETE") == 0)
        request.setType(DELETE);
    else
    {
        std::cout << "set_request_type::ERROR METHOD NOT KNOWN" << std::endl;
        // HANDLE ERROR
    }
}