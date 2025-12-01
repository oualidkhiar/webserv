
#ifndef HANDLE_PASSIVE_SOCKETS_HPP
#define HANDLE_PASSIVE_SOCKETS_HPP

#include "./config.hpp"
#include "./constent.hpp"
#include "./socketsManager.hpp"
// #include "./client.hpp"
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>

class ListeningSocket: public socketsManager {
private:

    // serverConfig                                    *serverConf;
    // int                                             ListenFd;
    int     epfd;

public:

    ListeningSocket(serverConfig *conf, int fd, ServerManager *ptr);
    ~ListeningSocket();

    void handleSocketsAction();

};

#endif