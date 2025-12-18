#include "RequestParser.hpp"
#include "enums.hpp"
#include "StringManip.hpp"
#include <iostream>

RequestParser ::RequestParser() {}

void RequestParser::reading_request_line(HttpRequest &request)
{
    int token_numbers = 0;

    std::string &request_string = request.getRequest();
    size_t pos = request_string.find("\r\n");
    std::string line = request_string.substr(0, pos);
    std::string token;
    while ((token = StringManip::get_token(line, ' ')).empty() == false)
    {
        token_numbers++;
        if (token_numbers == 1)
            set_request_type(request , token);
        else if (token_numbers == 2)
            request.setUri(token);
        // else if (token_numbers = == 3) SET HTTP VERSION IF NEEDED , ELSE CHECK AND PASS
        //             request.setVersion();
        else if (token_numbers > 3)
        {
            std::cout << "reading_request_line::ERROR ARGS MORE THAN EXEPECTED" << std::endl;
            return;
        }
    }
    if (token_numbers == 3)
        request.setStatus(READ_HEADER);
}

void RequestParser::read_header(HttpRequest &request)
{
    
}
void RequestParser::create_request(HttpRequest &request)
{
    if (request.getStatus() == FINISHED)
        return;
    if (request.getStatus() == READ_HEADER)
        read_header(request);
    else if (request.getStatus() == READING_REQUEST_LINE)
        reading_request_line(request);
    else
        read_body(request);
}

void RequestParser::set_request_type(HttpRequest &request, std::string token)
{
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