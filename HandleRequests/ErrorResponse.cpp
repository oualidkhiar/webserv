#include "ErrorResponse.hpp"
#include "utils.hpp"
#include <iostream>
#include <fstream>
#include <sstream>
#include <cstring>

std::map<int, std::string> ErrorResponse::initErrorPages()
{
    std::map<int, std::string> errorPages;
    
    errorPages[400] = "www/error_pages/400.html";
    errorPages[403] = "www/error_pages/403.html";
    errorPages[404] = "www/error_pages/404.html";
    errorPages[405] = "www/error_pages/405.html";
    errorPages[413] = "www/error_pages/413.html";
    errorPages[414] = "www/error_pages/414.html";
    errorPages[500] = "www/error_pages/500.html";
    errorPages[501] = "www/error_pages/501.html";
    errorPages[503] = "www/error_pages/503.html";
    errorPages[505] = "www/error_pages/505.html";
    errorPages[504] = "www/error_pages/504.html";
    
    return errorPages;
}

void mergeContentWithHeaders(size_t& bodyLen, std::string& content, int code) {
    std::string headers;
    std::ostringstream s;
	s << bodyLen;
    headers = getStatusReponseLine(code);
    headers += "Content-Type: text/html\r\n"
            "Content-Length: "+s.str()+"\r\n"
            "Connection: close\r\n" // im not sure 
            "\r\n";
    content = headers+content;
    bodyLen = content.size();
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
    }

    size_t size = content.size();
    mergeContentWithHeaders(size, content, code);
    unsigned char *response = new unsigned char[size]; // need to freethis.
    if (size > 0)
        std::memcpy(response, content.c_str(), size);

    return std::make_pair(response, size);
}
