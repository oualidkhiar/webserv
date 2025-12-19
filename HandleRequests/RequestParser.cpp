#include "RequestParser.hpp"
#include "enums.hpp"
#include "StringManip.hpp"
#include <iostream>
#include <cstdlib>

RequestParser ::RequestParser() {}

void RequestParser::read_body(HttpRequest &request)
{
    std::string value;
    int size;
    std::string &request_string = request.getRequest();
    if ((value = request.getHeader(CHUNKED)).empty() == false)
    {
        size_t pos = request_string.find('\n');
        size = hex_to_num(request_string.substr(0, pos));
        request_string.erase(0, pos + 1);
        std::string chunk = request_string.substr(0, size);
        if (request_string.at(pos) == '\n')
            request_string.erase(0 , pos + 1);
        else
        {
            std::cout << "BAD REQUEST FOR NOW" << std::endl;
            exit(1);
        }
    }
    else if ((value = request.getHeader(FIXED_LENGTH)).empty() == false)
    {
        request.set
    }
    else
    {
        std::cout << "BAD REQUEST FOR NOW" << std::endl;
        exit(1);
    }
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