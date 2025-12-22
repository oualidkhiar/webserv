#pragma once
#include <string>
#include <map>
#include "enums.hpp"
#include "Body.hpp"

class HttpRequest

{
private:
    std::string &request;
    enum RequestType type;
    std::string uri;
    std::map<std::string, std::string> headers;
    enum status status;
    int response_code;
    Body *body;
    size_t available_data;

public:
    HttpRequest(std ::string &_request);
    enum RequestType getType();
    std::string getUri();
    std::map<std::string, std::string> getHeaders();
    std::string getHeader(std::string key);
    enum status getStatus();
    std::string &getRequest();
    int getResponseCode();
    Body *getBody();
    int addHeader(std::string key, std::string value);
    size_t getAvailableData();

    void setAvailableData(size_t available_data);
    void setBody(Body *body);
    void setResponseCode(int code);
    void setHeaders(std::map<std::string, std::string> headers);
    void setUri(std::string uri);
    void setType(enum RequestType type);
    void setStatus(enum status status);

    ~HttpRequest();
};