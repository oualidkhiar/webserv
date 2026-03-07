#include "ErrorResponse.hpp"
#include "utils.hpp"
#include <iostream>
#include <fstream>
#include <sstream>
#include <cstring>
#include "403_html.h"
#include "404_html.h"
#include "405_html.h"
#include "413_html.h"
#include "500_html.h"
#include "503_html.h"
#include "504_html.h"

static void mergeContentWithHeaders(size_t &bodyLen, std::string &content, int code, std ::string hostname = "")
{
    std::string headers;
    std::ostringstream s;
    s << bodyLen;
    headers = getStatusReponseLine(code);
    headers += "server: " + hostname + "\r\n"
                                       "Content-Type: text/html\r\n"
                                       "Content-Length: " +
               s.str() + "\r\n"
                         "Connection: close\r\n"
                         "\r\n";
    content = headers + content;
    bodyLen = content.size();
}

unsigned char *getDefaultErrorPages(int code)
{
    switch (code)
    {
    case 403:
        return __403_html;
    case 404:
        return __404_html;
    case 405:
        return __405_html;
    case 413:
        return __413_html;
    case 500:
        return __500_html;
    case 503:
        return __503_html;
    case 504:
        return __504_html;
    default:
        break;
    }
    return (unsigned char *)"";
}

std::pair<unsigned char *, size_t> ErrorResponse::getErrorResponse(serverConfig *serverConf, int code)
{
    std::string content;
    std::string hostname = "";
    if (serverConf->getServerNames().empty() == false)
    {
        hostname = serverConf->getServerNames()[0];
    }
    std::map<int, std::string> &errorPages = serverConf->getErrorPages(); // user error pages
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
    else
    {
        content.assign((char *)getDefaultErrorPages(code));
    }
    size_t size = content.size();
    mergeContentWithHeaders(size, content, code, hostname);
    unsigned char *response = new unsigned char[size]; // need to freethis.
    if (size > 0)
        std::memcpy(response, content.c_str(), size);

    return std::make_pair(response, size);
}
