#include "../includes/handle_passive_sockets.hpp"
#include "../includes/server_manager.hpp"

ListeningSocket::ListeningSocket(serverConfig *conf, int fd, ServerManager *ptr): socketsManager(conf, fd, ptr) {}

ListeningSocket::~ListeningSocket() {
    // close();
}

void ListeningSocket::handleSocketsAction( void )
{
    struct epoll_event  ev;
    struct sockaddr     address;
    socklen_t           addLen;

    while (true) {
        int clientFd = accept(this->socketFd, (struct sockaddr *)(&address), &addLen);
        socketsManager *sock = new ClientSocket(this->serverConf, clientFd);
        ev.events = EPOLLIN | EPOLLOUT;
        ev.data.ptr = sock;
        if (clientFd < 0) {
            if (errno == EAGAIN || errno == EWOULDBLOCK) {
                break ;
            }
            else {
                perror ("accept failed");
                break ;
            }
        }
        epoll_ctl(this->epfd, EPOLL_CTL_ADD, clientFd, &ev);
        // this->ptr->MapPointers().insert({clientFd, sock});
    }
}