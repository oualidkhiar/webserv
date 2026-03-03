#pragma once
#include <string>
#include "HttpRequest.hpp"
#include "HttpResponse.hpp"

class RequestParser
{
private:
    void reading_request_line(HttpRequest &request, HttpResponse &response);
    void read_header(HttpRequest &request, HttpResponse &response);
    bool set_request_type(HttpRequest &request, std::string token);
    long long get_chunked_size(HttpRequest &request, const size_t &pos);
    bool get_chunked(HttpRequest &request, long long size);
    void read_body_fixed(HttpRequest &request);
    bool setBufferFixed(HttpRequest &request , Body * body);
    size_t findCrlfPos(HttpRequest & request);
public:
    RequestParser();
    void create_request(HttpRequest &request, HttpResponse &response);
    void read_body_chunked(HttpRequest &request); // to be private (public just for debugging)
    void read_body(HttpRequest &request);// to be private (public just for debugging)
    static size_t find2CrlfPos(HttpRequest &request);
};