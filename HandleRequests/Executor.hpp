#pragma once
#include "HttpRequest.hpp"
#include "HttpResponse.hpp"
#include "../includes/config.hpp"
class Executor
{
private:

    // methods // 
    void setContentTpe(HttpResponse & response , const std::string &path);
    std::string pathResolver(HttpRequest & request);
    bool isAllowedMethod(HttpRequest &request);
    void setLocation(HttpRequest &request);
    int matchedScore(std::string uri, std::string key);
    void executeDelete(HttpRequest &request, HttpResponse & response);
    location *getLongestMatchedLocation(HttpRequest &request, std::map<std::string, location *> map);

    public:
    std::pair<int , FtFile *> extractFileInfos(const char * path);
    void executeGet(HttpRequest &request, HttpResponse & response);
    Executor();
    void execute(HttpRequest &request , HttpResponse & response);
};

