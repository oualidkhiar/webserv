#include "../includes/clientSocket.hpp"

ClientSocket::ClientSocket(int fd ,serverConfig *conf, ServerManager *ptr): 
socketsManager(conf, fd, ptr), state(READING_REQUEST) {}

void ClientSocket::handleRequest( void ) {

}

void ClientSocket::handleResponse( void ) {
	
}

void ClientSocket::handleEvent() {
	
}