#include "MimeTypes.hpp"
#include "enums.hpp"

std::map<std::string, std::string> MimeTypes::types;

void MimeTypes::init()
{
    if (types.empty() == true)
        return;
    types["html"] = "text/html";
    types["htm"] = "text/html";
    types["css"] = "text/css";
    types["js"] = "text/javascript";
    types["png"] = "image/png";
    types["jpg"] = "image/jpeg";
    types["jpeg"] = "image/jpeg";
    types["gif"] = "image/gif";
    types["json"] = "application/json";
    types["txt"] = "text/plain";
}
std::string MimeTypes::getType(const std::string &extention)
{
    std::map<std::string, std::string>::const_iterator it;
    it = types.find(extention);
    if (it != types.end())
        return (it->second);
    return DEFAULT_CONTENT_TYPE;
}