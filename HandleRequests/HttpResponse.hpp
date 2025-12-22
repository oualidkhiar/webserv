#pragma once
#include "enums.hpp"
#include <map>
#include <string>
#include "HttpRequest.hpp"
class HttpResponse
{
private:
    int status;
    std::map<std::string, std::string> headers;
    std::string body;

public:
    HttpResponse();
    void setStatus(int status);
    void AddHeader(std ::string key, std::string value);
    void setBody(std ::string body);

    std::string getReasonPhrase(int code);
    int getStatus();
    std::string getHeader(std::string key);
    std::string getBody();
    static HttpResponse getResponse(HttpRequest & request);
    ~HttpResponse();
};