#include "RequestParser.hpp"
#include "enums.hpp"

RequestParser ::RequestParser() {}

void RequestParser::reading_request_line(HttpRequest &request)
{
    std::string &request_string = request.getRequest();
    size_t pos = request_string.find("\r\n");
    std::string line = request_string.substr(0, pos);
    
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