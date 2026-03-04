#include "listeningSocket.hpp"
#include "server_manager.hpp"
#include <arpa/inet.h>

ListeningSocket::ListeningSocket(serverConfig *conf, int fd): socketsManager(conf, fd) {}

ListeningSocket::~ListeningSocket() {}

std::string getMergedIpPort(struct sockaddr_in addr)
{
    char ip[INET_ADDRSTRLEN];
    inet_ntop(AF_INET, &addr.sin_addr, ip, INET_ADDRSTRLEN);
    std::ostringstream ipStr;
    ipStr << ip;
    std::ostringstream portStr;
    portStr << ntohs(addr.sin_port);
    std::string res = ipStr.str()+":"+portStr.str();
    return res;
}

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
        DisplyLogs::printCurrentAtion("[CONN ] [ACCEPT ]", "Client "+getMergedIpPort(address)+" connected", GREEN);
    }
    this->action = ADD_CONNECTIONS;
}