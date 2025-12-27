#include "../includes/clientSocket.hpp"
#include "../includes/server_manager.hpp"

ClientSocket::ClientSocket(int fd ,serverConfig *conf, ServerManager *ptr): 
socketsManager(conf, ptr, fd), state(READING_REQUEST) {}

// void ClientSocket::handleRequest( void ) {

// }

// void ClientSocket::handleResponse( void ) {
	
// }

void ClientSocket::continueReading()
{
    char *buffer = new char[8001];
    int bytesRead;

    bytesRead = read(this->socketFd, buffer, 8000);
    if (bytesRead > 8000) {
        this->state = PROCESSING;
        this->ReceiveBuffer = buffer;
        delete[] buffer;
    }
    else {
        this->ReceiveBuffer += buffer;
        delete[] buffer;
    }
    std::cout << this->ReceiveBuffer <<std::endl;
}


void ClientSocket::handleEvent()
{
	// if (this->state == READING_REQUEST) {
        continueReading();
    // }
    // else if (this->state == PROCESSING) {
    //     continueProcessign();
    // }
    // else if (this->state == WRITING_RESPONSE) {
    //     continueWriting();
    // }
    // if (this->state == CLOSED) {
    //     this->ptr->removeConnection(this->socketFd);
    // }
}