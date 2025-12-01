#include "../includes/client.hpp"

ClientSocket::ClientSocket(serverConfig *conf, int fd): socketsManager(conf, fd) {
	RequestIsFull = false;
	ResponseIsComplet = false;
}

int handleRequest( void ) {

}

int handleResponse( void ) {
	
}

void handleSocketsAction() {

}