
#include "utils.hpp"
#include <iostream>
#include <ctime>

std::string getStatusReponseLine(int code)
{
    std::string statusLine;
    switch (code)
    {
    case HP_OK:
        statusLine = "HTTP/1.1 200 OK\r\n";
        break;
    case HP_CREATED:
        statusLine = "HTTP/1.1 201 Created\r\n";
        break;
    case HP_NO_CONTENT:
        statusLine = "HTTP/1.1 204 No Content\r\n";
        break;
    case HP_MOVED_PERMANENTLY:
        statusLine = "HTTP/1.1 301 Moved Permanently\r\n";
        break;
    case HP_BAD_REQUEST:
        statusLine = "HTTP/1.1 400 Bad Request\r\n";
        break;
    case HP_FORBIDDEN:
        statusLine = "HTTP/1.1 403 Forbidden\r\n";
        break;
    case HP_NOT_FOUND:
        statusLine = "HTTP/1.1 404 Not Found\r\n";
        break;
    case HP_METHOD_NOT_ALLOWED:
        statusLine = "HTTP/1.1 405 Method Not Allowed\r\n";
        break;
    case HP_PAYLOAD_TOO_LARGE:
        statusLine = "HTTP/1.1 413 Payload Too Large\r\n";
        break;
    case HP_INTERNAL_SERVER_ERROR:
        statusLine = "HTTP/1.1 500 Internal Server Error\r\n";
        break;
    case HP_NOT_IMPLEMENTED:
        statusLine = "HTTP/1.1 501 Not Implemented\r\n";
        break;
    case HP_VERSION_NOT_SUPPORTED:
        statusLine = "HTTP/1.1 505 HTTP Version Not Supported\r\n";
        break;
    default:
        statusLine = "HTTP/1.1 500 Internal Server Error\r\n";
        break;
    }
    return statusLine;
}

// Sat, 03 Jan 2026 11:10:45 GMT
std::string getDateValue()
{
}

int main()
{
    std::time_t result = std::time(NULL);
    char *date;
    date = std::asctime(std::localtime(&result));
    std::cout << date << std::endl;
}