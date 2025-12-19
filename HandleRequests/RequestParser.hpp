#pragma once
#include <string>
#include "HttpRequest.hpp"

class RequestParser
{
private:
    void reading_request_line(HttpRequest &request);
    void read_header(HttpRequest &request);
    void read_body(HttpRequest &request);
    void set_request_type(HttpRequest &request , std::string token);
public:
    RequestParser();
    void create_request(HttpRequest &request);
};