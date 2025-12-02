#include "../includes/listeningSocket.hpp"
#include "../includes/server_manager.hpp"

ListeningSocket::ListeningSocket(serverConfig *conf, int fd, ServerManager *ptr): socketsManager(conf, fd, ptr) {}

ListeningSocket::~ListeningSocket() {

}

void ListeningSocket::handleEvent( void )
{
    struct epoll_event  ev;
    struct sockaddr     address;
    socklen_t           addLen;

    addLen = sizeof(address);
    while (true) {
        int clientFd = accept(this->socketFd, (struct sockaddr *)(&address), &addLen);
        socketsManager *sock = new ClientSocket(clientFd, this->serverConf, this->ptr);
        ev.events = EPOLLIN;
        ev.data.ptr = sock;
        if (clientFd < 0) {
            if (errno == EAGAIN || errno == EWOULDBLOCK) {
                break ;
            }
            else {
                perror ("accept failed");
                // this->ptr->setError();  i wanna make sure first what we gonna do in this case (continue or exit) 
                break ;
            }
        }
        this->ptr->addConnection(ev, clientFd, sock);
    }
}