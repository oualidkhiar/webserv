#pragma once
#include <map>
#include <string>
class HttpResponse
{
private:
    int status;
    std::map<std::string, std::string> headers;
    std::string body;

public:
    HttpResponse();
    void setStatus(int status);
    int AddHeader(std ::string key, std::string value);
    void setBody(std ::string body);

    int getStatus();
    std::string getHeader(std::string key);
    std::string getBody();
    ~HttpResponse(){}
};