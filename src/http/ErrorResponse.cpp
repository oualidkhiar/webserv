#include "ErrorResponse.hpp"
#include "utils.hpp"
#include <iostream>
#include <fstream>
#include <sstream>
#include <cstring>

static void mergeContentWithHeaders(size_t& bodyLen, std::string& content, int code) {
    std::string headers;
    std::ostringstream s;
	s << bodyLen;
    headers = getStatusReponseLine(code);
    headers += "Content-Type: text/html\r\n"
            "Content-Length: "+s.str()+"\r\n"
            "Connection: close\r\n"
            "\r\n";
    content = headers+content;
    bodyLen = content.size();
}

static void buildSimpleHtmlForShowingErrorNumber(std::string& content, int code) // in case we dont have an error pages for that error
{
    std::string header_line = getStatusReponseLine(code);
    if (header_line.empty()) {
        std::ostringstream s;
        s << code;
        header_line = "Unknow error Number: "+s.str();
    }
    content = "<!DOCTYPE html>\n"
                "<html lang=\"en\">\n"
                "<head>\n"
                "    <meta charset=\"UTF-8\">\n"
                "    <meta name=\"viewport\" content=\"width=device-width, initial-scale=1.0\">\n"
                "    <title>" + header_line + "</title>\n"
                "    <style>\n"
                "        body {\n"
                "            font-family: 'Segoe UI', Tahoma, Geneva, Verdana, sans-serif;\n"
                "            background: linear-gradient(135deg, #667eea 0%, #764ba2 100%);\n"
                "            min-height: 100vh;\n"
                "            display: flex;\n"
                "            justify-content: center;\n"
                "            align-items: center;\n"
                "            color: white;\n"
                "        }\n"
                "        .error-code {\n"
                "            font-size: 150px;\n"
                "            font-weight: bold;\n"
                "            line-height: 1;\n"
                "            margin-bottom: 20px;\n"
                "            text-shadow: 4px 4px 8px rgba(0, 0, 0, 0.3);\n"
                "            animation: bounce 2s infinite;\n"
                "        }\n"
                "    </style>\n"
                "</head>\n"
                "<body>\n"
                "    <div class=\"error-code\">"+header_line+"</div>\n"
                "</body>\n"
                "</html>";
}

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
    } else {
        buildSimpleHtmlForShowingErrorNumber(content, code);
    }

    size_t size = content.size();
    mergeContentWithHeaders(size, content, code);
    unsigned char *response = new unsigned char[size]; // need to freethis.
    if (size > 0)
        std::memcpy(response, content.c_str(), size);

    return std::make_pair(response, size);
}
