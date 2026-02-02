#include "../includes/clientSocket.hpp"
#include "../includes/server_manager.hpp"

ClientSocket::ClientSocket(int fd ,serverConfig *conf, ServerManager *ptr): 
socketsManager(conf, ptr, fd), state(READING_REQUEST) {}

void ClientSocket::readingAndProcessingRequest()
{
    char *buffer = new char[8000]();
    int bytesRead;

    bytesRead = read(this->socketFd, buffer, 8000);
    if (bytesRead > 0) {
        // call ilias's function for proccess request and see the status if the request complete or not
        // if (status == finish) : change the state for client to WRITING_RESPONSE
        // let the data for another call to handle it
        delete[] buffer;
    }
    else {
        delete[] buffer;
        // client closs the connection remove the connection
    }
}

void ClientSocket::continueWriting(std::string& response)
{
    int ret;

    ret = write(socketFd, response.data(), response.length());
    if (ret == -1) {
        std::cout << "here\n\n";
        this->ptr->removeConnection(this->socketFd);
    }
    else {
        this->ptr->modifyEvent(EPOLLIN, this->socketFd);
    }
}


void ClientSocket::handleEvent()
{
	if (this->state == READING_REQUEST) {
        readingAndProcessingRequest();
        std::cout << this->state << std::endl;
    }
    else if (this->state == WRITING_RESPONSE) {
        
        std::string response ="HTTP/1.1 200 OK\r\n""Content-Length: 5\r\n""Content-Type: text/plain\r\n""\r\n""Hello";
        continueWriting(response);
    }
}