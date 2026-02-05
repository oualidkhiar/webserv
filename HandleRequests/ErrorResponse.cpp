#include "ErrorResponse.hpp"
#include <fstream>
#include <sstream>
#include <cstring>
#include <iostream>

std::string ErrorResponse::getStatusMessage(int code)
{
    switch (code)
    {
    case 400:
        return "Bad Request";
    case 403:
        return "Forbidden";
    case 404:
        return "Not Found";
    case 405:
        return "Method Not Allowed";
    case 413:
        return "Payload Too Large";
    case 414:
        return "URI Too Long";
    case 500:
        return "Internal Server Error";
    case 501:
        return "Not Implemented";
    case 503:
        return "Service Unavailable";
    case 505:
        return "HTTP Version Not Supported";
    default:
        return "Error";
    }
}

std::string ErrorResponse::getDefaultErrorPage(int code)
{
    std::ostringstream oss;
    oss << "<!DOCTYPE html>\n"
        << "<html><head><meta charset=\"UTF-8\"><title>" << code << " " << getStatusMessage(code) << "</title></head>\n"
        << "<body><h1>" << code << " " << getStatusMessage(code) << "</h1></body></html>";
    return oss.str();
}

std::map<int, std::string> ErrorResponse::initErrorPages()
{
    std::map<int, std::string> errorPages;
    
    errorPages[403] = "www/error_pages/403.html";
    errorPages[404] = "www/error_pages/404.html";
    errorPages[413] = "www/error_pages/413.html";
    errorPages[500] = "www/error_pages/500.html";
    errorPages[503] = "www/error_pages/503.html";
    
    return errorPages;
}

std::pair<unsigned char *, size_t> ErrorResponse::getErrorResponse(int code)
{
    static std::map<int, std::string> errorPages = initErrorPages();
    std::string content;
    
    std::map<int, std::string>::iterator it = errorPages.find(code);
    if (it != errorPages.end())
    {
        std::ifstream file(it->second.c_str());
        if (file.is_open())
        {
            std::ostringstream buffer;
            buffer << file.rdbuf();
            content = buffer.str();
            file.close();
        }
        else
        {
            content = getDefaultErrorPage(code);
        }
    }
    else
    {
        content = getDefaultErrorPage(code);
    }
    
    size_t size = content.size();
    unsigned char *response = new unsigned char[size];
    std::memcpy(response, content.c_str(), size);
    
    return std::make_pair(response, size);
}
