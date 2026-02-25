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
    int get_chunked_size(HttpRequest &request, const size_t &pos);
    bool get_chunked(HttpRequest & request,int size);
    void read_body_fixed(HttpRequest &request);
    bool setBufferFixed(HttpRequest &request , Body * body);
    size_t findCrlfPos(HttpRequest & request);
    size_t find2CrlfPos(HttpRequest &request);
public:
    RequestParser();
    void create_request(HttpRequest &request);
    void read_body_chunked(HttpRequest &request); // to be private (public just for debugging)
};