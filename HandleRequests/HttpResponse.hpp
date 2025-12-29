#pragma once
#include <vector>
#include "enums.hpp"
#include <map>
#include <string>
#include "FtFile.hpp"
#include "HttpRequest.hpp"
#include "Body.hpp"


class HttpResponse
{
private:
    int status;
    std::map<std::string, std::string> headers;
    Body * body;
    enum status state;
    FtFile *file;


public:
    HttpResponse();
    void setStatus(int status);
    void AddHeader(std ::string key, std::string value);
    void setFile(FtFile *file);


    void appendBodyToResponse(std::vector<unsigned char> &chunk);
    FtFile *getFile();
    std::string getReasonPhrase(int code);
    int getStatus();
    std::string getHeader(std::string key);
    static HttpResponse getResponse(HttpRequest &request);
    ~HttpResponse();
};