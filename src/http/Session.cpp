#include "Session.hpp"

Session::Session(std::string id, std::time_t expiration) : session_id(id), expiration_time(expiration) {}
std::string Session::getSessionId() { return session_id; }
void Session::setData(std::string key, std::string value) { data[key] = value; }
std::string Session::getData(std::string key) {
    std::map<std::string, std::string>::iterator it = data.find(key);
    if (it != data.end())
        return it->second;
    return "";
}   
void Session::setExpirationTime(std::time_t expiration) { this->expiration_time = expiration; }
bool Session::isExpired() { return std::time(0) > expiration_time; }
Session::~Session() {}
