#ifndef SERVER_MANAGER_HPP
#define SERVER_MANAGER_HPP

#include "./handle_passive_sockets.hpp"
#include "./client.hpp"
#include <sys/epoll.h>


class ServerManager{
private:

    std::unordered_map<int, socketsManager *>   socketHandler;
    struct epoll_event                          events[MAX_EVENTS];
    config                                      conf;
    int                                         epfd;
    bool                                        error;

public:

    ServerManager(std::string FileConfigName);
    ~ServerManager();
    void StartAllServers();
    void TrackSocketsEvent();



    // int GetEpollFd();
    // struct epoll_event *GetEventEpoll();
    // std::unordered_map<int, void *> GetSocketMap();
    // std::vector<SocketInfo *> GetSocketInfoContiner();
    // void    freeClientData(int fd);
    // bool checkError();

};

#endif