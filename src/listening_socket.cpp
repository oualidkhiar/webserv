#include "../includes/listeningSocket.hpp"
#include "../includes/server_manager.hpp"

ListeningSocket::ListeningSocket(serverConfig *conf, int fd): socketsManager(conf, fd) {}

ListeningSocket::~ListeningSocket() {}

void ListeningSocket::handleEvent( void )
{
    struct sockaddr_in     address;
    socklen_t           addLen;

    addLen = sizeof(address);
    while (true) {

        int clientFd = accept(this->socketFd, (struct sockaddr *)(&address), &addLen);
        if (clientFd < 0) {
            if (errno == EAGAIN || errno == EWOULDBLOCK) {
                break ;
            }
            else {
                perror ("accept failed");
                break ;
            }
        }
        fcntl(clientFd, F_SETFL, O_NONBLOCK);
        socketsManager *sock = new ClientSocket(clientFd, this->serverConf);
        this->newClientFds.push_back(sock);
    }
    this->action = ADD_CONNECTIONS;
}