#ifndef SOCKETSMANAGER_HPP
#define SOCKETSMANAGER_HPP

#include "./config.hpp"
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <ctime>
class ServerManager;

class socketsManager {
protected:

    serverConfig    *serverConf;
    ServerManager   *ptr;
    int              socketFd;

public:

    socketsManager(serverConfig *conf, ServerManager *ptr, int fd);
    virtual ~socketsManager();

    virtual void handleEvent() = 0;
    // virtual void printLog() = 0;

};


#endif