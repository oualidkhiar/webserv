#pragma once
#include <vector>
#include "enums.hpp"
#include <map>
#include <string>
#include <FtFile.hpp>
#include "HttpRequest.hpp"


class HttpResponse
{
private:
    int status;
    std::map<std::string, std::string> headers;
    std::vector<char> body;
    enum status state;
    FtFile *file;


public:
    HttpResponse();
    void setStatus(int status);
    void AddHeader(std ::string key, std::string value);
    void setBody(std::vector<char> body);
    void setFile(FtFile *file);


    FtFile *getFile();
    std::string getReasonPhrase(int code);
    int getStatus();
    std::string getHeader(std::string key);
    std::vector<char> getBody();
    static HttpResponse getResponse(HttpRequest &request);
    ~HttpResponse();
};