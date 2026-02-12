#ifndef SERVER_MANAGER_HPP
#define SERVER_MANAGER_HPP

#include "./listeningSocket.hpp"
#include "./clientSocket.hpp"
#include <sys/epoll.h>


class ServerManager {
private:

    std::map<int, socketsManager *>             socketHandler;
    struct epoll_event                          events[MAX_EVENTS];
    config&                                     conf;
    int                                         epfd;
    bool                                        error;

    int ListeningSocketStart(int port);

public:

    ServerManager(config& conf);
    ~ServerManager();

    void StartAllServers();
    void TrackSocketsEvent();

    void addConnection(socketsManager * client);
    void removeConnection(int fd);
    void modifyEvent(int state, int fd, struct epoll_event& ev);

    void setError();
    bool checkError( void );

};

#endif