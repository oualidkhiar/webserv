#pragma once
#include <string>
#include "HttpRequest.hpp"

class RequestParser
{
private:
    void reading_request_line(HttpRequest &request);
    void read_header(HttpRequest &request);
    void read_body(HttpRequest &request);
    bool set_request_type(HttpRequest &request, std::string token);
    void read_body_chunked(HttpRequest &request);
    bool get_chunked(HttpRequest & request,int size);
    void read_body_fixed(HttpRequest &request);
    bool setBufferFixed(HttpRequest &request , Body * body);
    size_t findCrlfPos(HttpRequest & request);
    size_t findEndOfHeader(HttpRequest & request);
public:
    RequestParser();
    void create_request(HttpRequest &request);
};