#include "MimeTypes.hpp"
#include "enums.hpp"
#include <iostream>
std::map<std::string, std::string> MimeTypes::types;

void MimeTypes::init()
{
    if (types.size() > 0)
        return;
    types["html"] = "text/html\r\n";
    types["htm"] = "text/html\r\n";
    types["css"] = "text/css\r\n";
    types["js"] = "text/javascript\r\n";
    types["png"] = "image/png\r\n";
    types["jpg"] = "image/jpeg\r\n";
    types["jpeg"] = "image/jpeg\r\n";
    types["gif"] = "image/gif\r\n";
    types["json"] = "application/json\r\n";
    types["txt"] = "text/plain\r\n";
    types["mp4"] = "video/mp4\r\n";
}
std::string MimeTypes::getType(const std::string &extention)
{
    init();
    std::map<std::string, std::string>::const_iterator it;
    std::string content_type;
    it = types.find(extention);
    if (it != types.end())
        content_type = it->second;
    else
        content_type = DEFAULT_CONTENT_TYPE;
    return (content_type);
}