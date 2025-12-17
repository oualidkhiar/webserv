#pragma once
#include <string>
#include <map>
#include "enums.hpp"

class HttpRequest

{
private:
    std::string &request;
    enum RequestType type;
    std::string uri;
    std::map<std::string, std::string> headers;
    std::string body;
    enum status status;

public:
    HttpRequest(std :: string & _request);
    enum RequestType getType();
    std::string getUri();
    std::map<std::string, std::string> getHeaders();
    std::string getBody();
    std::string getHeader(std::string key);
    enum status getStatus();
    std::string &getRequest();

    void setBody(std::string body);
    void setHeaders(std::map<std::string, std::string> headers);
    void setUri(std::string uri);
    void setType(enum RequestType type);
    void setStatus(enum status type);
};