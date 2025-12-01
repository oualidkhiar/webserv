#ifndef SOCKETSMANAGER_HPP
#define SOCKETSMANAGER_HPP

#include "./config.hpp"

class socketsManager {
protected:

    serverConfig    *serverConf;
    ServerManager   *ptr;
    int              socketFd;

public:

    socketsManager(serverConfig *conf, int fd, ServerManager *ptr): serverConf(conf), socketFd(fd), ptr(ptr) {}
    virtual ~socketsManager();

    virtual void handleSocketsAction() = 0;

};


#endif