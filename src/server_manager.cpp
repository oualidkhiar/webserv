#include "../includes/server_manager.hpp"

ServerManager::ServerManager(config& conf): conf(conf), error(false) {}

ServerManager::~ServerManager() {
    for (std::map<int, socketsManager *>::iterator it = socketHandler.begin(); it != socketHandler.end(); it++) {
        close(it->first);
        delete it->second;
    }
    close(epfd);
}

int ServerManager::ListeningSocketStart(int port)
{
    int                 opt;
    int                 socketFd;
    struct sockaddr_in  address;
    socklen_t           addLen;

    address.sin_port = htons(port);
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    addLen = sizeof(address);
    socketFd = socket(AF_INET, SOCK_STREAM, 0);
    opt = 1;
    if (socketFd < 0) {
        perror("socket failed");
        return (-1);
    }
    if (setsockopt(socketFd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) < 0) {
        perror("setsockopt failed");
        close(socketFd);
        return -1;
    }
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
    struct epoll_event ev;

    this->epfd = epoll_create(1);
    if (this->epfd < 0) {
        perror("epoll_create failed");
        this->error = true;
        return;
    }
    for (int i = 0; i < conf.ServersNumber(); i++) {
        serverConfig *serverconf = conf.getSerevrConfig(i);
        for (size_t j = 0; j < serverconf->Port.size(); j++) {
            int sockFd = ListeningSocketStart(serverconf->Port[j]);
            if (sockFd < 0) {
                this->error = true;
                return;
            }
            int flags = fcntl(sockFd, F_GETFL, 0);
            fcntl(sockFd, F_SETFL, flags | O_NONBLOCK);
            socketsManager *sock = new ListeningSocket(serverconf, sockFd, this);
            ev.events = EPOLLIN;
            ev.data.ptr = sock;
            if (epoll_ctl(this->epfd, EPOLL_CTL_ADD, sockFd, &ev) < 0) {
                perror("epoll_ctl failed");
                close(sockFd);
                delete sock;
                this->error = true;
                return;
            }
            this->socketHandler.insert(std::make_pair(sockFd, sock));
        }
    }
}

void ServerManager::TrackSocketsEvent()
{
    int     nfds;

    while (true)
    {
        nfds = epoll_wait(epfd, events, MAX_EVENTS, -1);
        for (int i = 0; i < nfds; i++) {
            this->currentEv = events[i];
            socketsManager *sock = (socketsManager *)events[i].data.ptr;
            sock->handleEvent();
            if (this->error)
                break ;
        }
    }
}

void ServerManager::removeConnection(int fd) {
    socketsManager *sock = socketHandler[fd];
    epoll_ctl(epfd, EPOLL_CTL_DEL, fd, NULL);
    close(fd);
    delete sock;
    socketHandler.erase(fd);
}

void ServerManager::addConnection(struct epoll_event& ev, int fd, socketsManager *sock) {
    epoll_ctl(this->epfd, EPOLL_CTL_ADD, fd, &ev);
    this->socketHandler.insert(std::make_pair(fd, sock));
}

void ServerManager::modifyEvent(int state, int fd)
{
    this->currentEv.events = state;
    epoll_ctl(this->epfd, EPOLL_CTL_MOD, fd, &currentEv);
}

void ServerManager::setError() {
    this->error = true;
}

bool ServerManager::checkError( void ) {
    return this->error;
}
