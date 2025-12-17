#pragma once
#include <string>
#include <HttpRequest.hpp>

class RequestParser
{
private:
    void reading_request_line(HttpRequest &request);
    void read_header(HttpRequest &request);
    void read_body(HttpRequest &request);

public:
    RequestParser();
    void create_request(HttpRequest &request);
};