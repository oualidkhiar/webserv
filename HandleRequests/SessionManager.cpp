#include "SessionManager.hpp"
#include <iostream>
#include <sstream>
#include "utils.hpp"
#include "Session.hpp"
SessionManager::SessionManager() {}
Session* SessionManager::createSession(std::time_t expiration)
{
    std::string session_id;
    session_id = generateRandomName();
    Session* session = new Session(session_id, std::time(0) + expiration);
    sessions[session_id] = session;
    return session;
}

Session* SessionManager::getSession(std::string session_id)
{
    std::map<std::string, Session*>::iterator it = sessions.find(session_id);
    if (it != sessions.end())
        return it->second;
    return NULL;
}
void SessionManager::deleteSession(std::string session_id)
{
    std::map<std::string, Session*>::iterator it = sessions.find(session_id);
    if (it != sessions.end())
    {
        delete it->second;
        sessions.erase(it);
    }
}
SessionManager::~SessionManager()
{
    for (std::map<std::string, Session*>::iterator it = sessions.begin(); it != sessions.end(); it++)
    {
        delete it->second;
    }
    sessions.clear();
}
