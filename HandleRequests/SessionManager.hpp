#pragma once
#include <map>
#include <string>
#include <ctime>
class Session;

class SessionManager
{
private:
    std::map<std::string, Session*> sessions;
public:
    SessionManager();
    Session* createSession(std::time_t expiration);
    Session* getSession(std::string session_id);
    void deleteSession(std::string session_id);
    ~SessionManager();
    

    /* data */

};