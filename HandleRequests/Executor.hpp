#pragma once
#include "HttpRequest.hpp"
#include "HttpResponse.hpp"
#include "../includes/config.hpp"
class Executor
{
private:
    std::vector<serverConfig *> *servers; // pointer to all servers configs 

    // methods // 
    void setContentTpe(HttpResponse & response , const std::string &path);
    std::pair<int , FtFile *> extractFileInfos(const char * path);
    std::string pathResolver(HttpRequest & request);
    bool isAllowedMethod(HttpRequest &request);
    void setLocation(HttpRequest &request);
    int matchedScore(std::string uri, std::string key);
    HttpResponse executeGet(HttpRequest &request);
    HttpResponse executeDelete(HttpRequest &request);
    location *getLongestMatchedLocation(HttpRequest &request, std::map<std::string, location *> map);

public:
    Executor(std::vector<serverConfig *> *servers);
    HttpResponse execute(HttpRequest &request);
};

