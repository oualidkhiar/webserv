#ifndef SERVER_MANAGER_HPP
#define SERVER_MANAGER_HPP

#include "./listeningSocket.hpp"
#include "./clientSocket.hpp"
#include <sys/epoll.h>


class ServerManager {
private:

    std::unordered_map<int, socketsManager *>   socketHandler;
    struct epoll_event                          events[MAX_EVENTS];
    config&                                     conf;
    int                                         epfd;
    bool                                        error;

    int ListeningSocketStart(serverConfig *serverConf, int port);

public:

    ServerManager(config& conf);
    ~ServerManager();

    void StartAllServers();
    void TrackSocketsEvent();

    void addConnection(struct epoll_event ev, int fd, socketsManager *sock);
    void removeConnection(int fd);

    void setError();
    bool checkError( void );

};

#endif