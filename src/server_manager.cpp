#include "../includes/server_manager.hpp"

ServerManager::ServerManager(std::string FileConfigName): conf(FileConfigName), error(false) {
    if (conf.CheckParse()) {
        this->error = true;
        return ;
    }
    epfd = epoll_create(0);
}

ServerManager::~ServerManager() {

}

int ListeningSocketStart(serverConfig *serverConf)
{
    int                 opt;
    int                 socketFd;
    struct sockaddr_in  address;
    socklen_t           addLen;

    address.sin_port = htons(serverConf->Port);
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    addLen = sizeof(address);
    socketFd = socket(AF_INET, SOCK_STREAM, 0);
    opt = 1;
    if (socketFd < 0) {
        perror("socket failed");
        return (-1);
    }
    setsockopt(socketFd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));
    if (bind(socketFd, (struct sockaddr *)(&address), addLen) == -1) {
        perror("bind failed");
        close(socketFd);
        return (-1);
    }
    if (listen(socketFd, BACKLOGMAX) == -1) {
        perror("listen failed");
        close (socketFd);
        return (-1);
    }
    return (socketFd);
}

void ServerManager::StartAllServers()
{
    struct epoll_event  ev;
    int sockFd;

    for (int i = 0; i < conf.ServersNumber(); i++) {
        if ((sockFd = ListeningSocketStart(conf.getSerevrConfig(i))) < 0) {
            this->error = true;
            return ;
        }
        socketsManager *sock = new ListeningSocket(conf.getSerevrConfig(i), sockFd, this);
        ev.data.ptr = sock;
        ev.events = EPOLLIN | EPOLLET;
        epoll_ctl(epfd, EPOLL_CTL_ADD, sockFd, &ev);
        this->socketHandler.insert({sockFd, sock});
    }
}

void ServerManager::TrackSocketsEvent()
{
    int     nfds;

    while (true)
    {
        nfds = epoll_wait(epfd, events, MAX_EVENTS, -1);
        for (int i = 0; i < nfds; i++) {
            socketsManager *sock = (socketsManager *)events[i].data.ptr;
            sock->handleSocketsAction();
        }
    }
}

//add item to the map
// insert_new_Record(pair<>) {
//     opu
// }

// int ServerManager::GetEpollFd() {
//     return this->epfd;
// }

// struct epoll_event *ServerManager::GetEventEpoll() {
//     return this->events;
// }

// std::unordered_map<int, void *> ServerManager::GetSocketMap() {
//     return this->socketHandler;
// }

// std::vector<SocketInfo *> ServerManager::GetSocketInfoContiner() {
//     return this->sockInfoAdd;
// }

// void    ServerManager::freeClientData(int fd) {
//     delete (ClientSocket *)(this->socketHandler[fd]);
//     this->socketHandler.erase(fd);
//     for (int i = 0; i < sockInfoAdd.size(); i++) {
//         if (sockInfoAdd[i]->fd == fd) {
//             delete sockInfoAdd[i];
//             sockInfoAdd.erase(sockInfoAdd.begin() + i);
//             break ;
//         }
//     }
//     close (fd);
// }

// bool ServerManager::checkError() {
//     return this->error;
// }