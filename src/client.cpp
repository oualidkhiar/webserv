#include "../includes/clientSocket.hpp"

ClientSocket::ClientSocket(int fd ,serverConfig *conf, ServerManager *ptr): 
socketsManager(conf, ptr, fd), state(READING_REQUEST) {}

void ClientSocket::handleRequest( void ) {

}

void ClientSocket::handleResponse( void ) {
	
}

void ClientSocket::handleEvent() {
	
}