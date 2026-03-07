#include "ErrorResponse.hpp"
#include "global.hpp"
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

std::string getErrorPagePath(HttpRequest& request)
{
    std::map<int, std::string>& errorPages = request.getConfig()->getErrorPages();
    if (errorPages.find(request.)) {

    }
}

std::pair<unsigned char *, size_t> ErrorResponse::getErrorResponse(serverConfig *serverConf)
{
    std::string fileErrorPath = getErrorPagePath(serverConf);
    std::string content;
    std::ifstream file(fileErrorPath.c_str());

    if (file.is_open())
    {
        std::ostringstream buffer;
        buffer << file.rdbuf();
        content = buffer.str();
        file.close();
    }
    size_t size = content.size();
    mergeContentWithHeaders(size, content, code);
    unsigned char *response = __404_html; // need to freethis.
    if (size > 0)
        std::memcpy(response, content.c_str(), size);

    return std::make_pair(response, size);
}
