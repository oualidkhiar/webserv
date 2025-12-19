#include "../includes/clientSocket.hpp"

ClientSocket::ClientSocket(int fd ,serverConfig *conf, ServerManager *ptr): 
socketsManager(conf, fd, ptr), state(READING_REQUEST) {}

int ClientSocket::handleRequest( void ) {

}

int ClientSocket::handleResponse( void ) {
	
}

void ClientSocket::handleEvent() {
	
}