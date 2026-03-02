#ifndef SOCKETSMANAGER_HPP
#define SOCKETSMANAGER_HPP

#include "./config.hpp"
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include "DisplyLogs.hpp"
class ServerManager;

enum Action {
    ADD_CONNECTIONS,
    CLOSE_CONNECTION,
    MODIFY_TO_READ,
    MODIFY_TO_WRITE,
    NO_ACTION
};

class socketsManager {
protected:

    serverConfig                     *serverConf;
    int                              socketFd;
    Action                           action;
    std::vector<socketsManager *>    newClientFds;

public:

    socketsManager(serverConfig *conf, int fd);
    virtual ~socketsManager();

    virtual void handleEvent() = 0;
    Action getAction() {return this->action;}
    void setActionNone( void ) {this->action = NO_ACTION;}
    int getFd() {return socketFd;}
    std::vector<socketsManager *>& getNewClient() {return this->newClientFds;}
    void clearVector() {newClientFds.clear();}

};


#endif