#ifndef SOCKETSMANAGER_HPP
#define SOCKETSMANAGER_HPP

#include "./config.hpp"
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include "DisplyLogs.hpp"
#include <ctime>
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
    time_t                           lastTimeInteraction;

public:

    socketsManager(serverConfig *conf, int fd);
    virtual ~socketsManager();

    virtual void handleEvent() = 0;
    virtual bool isTimeOut() = 0;
    Action getAction();
    void setActionNone( void );
    int getFd();
    std::vector<socketsManager *>& getNewClient();
    void clearVector();
    void updateTimeInteraction();

};


#endif