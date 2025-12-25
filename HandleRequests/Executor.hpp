#pragma once
#include "HttpRequest.hpp"
#include "HttpResponse.hpp"
#include "../includes/config.hpp"

class Executor
{
private:
    std::vector <serverConfig *> * servers;
    void setServer(HttpRequest & request);
    HttpResponse executeGet(HttpRequest &request);
    HttpResponse executeDelete(HttpRequest &request);
    location * getLongestMatchedLocation(std::string &fullpath, std::map < std::string, location *> map);

public:
    Executor(std::vector <serverConfig *> * servers);
    HttpResponse execute(HttpRequest &request);

};