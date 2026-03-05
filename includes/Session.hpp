#pragma once
#include <string>
#include <map>
#include <ctime>
#include "HttpResponse.hpp"
#include "HttpRequest.hpp"

class  Session
{
    private : 
    std::string session_id;
    std::map<std::string, std::string> data;
    std::time_t expiration_time;
    public :
    Session(std::string id, std::time_t expiration);
    std::string getSessionId();
    void setData(std::string key, std::string value);
    std::string getData(std::string key);
    void setExpirationTime(std::time_t expiration);
    bool isExpired();
    ~Session();

};