#include "ErrorResponse.hpp"
#include "utils.hpp"
#include "403_html.h"
#include "404_html.h"
#include "405_html.h"
#include "413_html.h"
#include "500_html.h"
#include "503_html.h"
#include "504_html.h"
#include "400_html.h"
#include "501_html.h"
#include <iostream>
#include <fstream>
#include <sstream>
#include <cstring>

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

std::pair<unsigned char *, size_t> getDefaultErrorPages(int code)
{
    switch (code)
    {
        case 403:
            return std::make_pair(__403_html, __403_html_len);
        case 404:
            return std::make_pair(__404_html, __404_html_len);
        case 405:
            return std::make_pair(__405_html, __405_html_len);
        case 413:
            return std::make_pair(__413_html, __413_html_len);
        case 500:
            return std::make_pair(__500_html, __500_html_len);
        case 503:
            return std::make_pair(__503_html, __503_html_len);
        case 504:
            return std::make_pair(__504_html, __504_html_len);
        case 400:
            return std::make_pair(__400_html, __400_html_len);
        case 501:
            return std::make_pair(__501_html, __501_html_len);
        default:
            break;
    }
    return std::make_pair((unsigned char *)"", 0);
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
    if (content.size() == 0)
    {
        std::pair<unsigned char *, size_t> p = getDefaultErrorPages(code);
        if (p.second > 0) {
            content.assign((char *)p.first, p.second);
        }
        else
        {
            std::ostringstream s;
            s << code;
            content = "<center>"
                            "<br><br><br><br><br>"
                            "<h1>"+s.str()+"</h1>"
                    "</center>";
        }
    }
    size_t size = content.size();
    mergeContentWithHeaders(size, content, code, hostname);
    unsigned char *response = new unsigned char[size]; // need to freethis.
    if (size > 0)
        std::memcpy(response, content.c_str(), size);

    return std::make_pair(response, size);
}
