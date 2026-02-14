#include "../includes/clientSocket.hpp"
#include "../includes/server_manager.hpp"
#include "../HandleRequests/ErrorResponse.hpp"

ClientSocket::ClientSocket(int fd ,serverConfig *conf): 
socketsManager(conf, fd), state(READING_REQUEST)
{
    this->transactionMgr = new TransactionManager();
    this->transactionMgr->setServer(conf);
}

ClientSocket::~ClientSocket() {
    delete this->transactionMgr;
}

void ClientSocket::readingAndProcessingRequest()
{
    unsigned char *buffer = new unsigned char[MAX_BUFFER_SIZE];
    int bytesRead;

    bytesRead = read(this->socketFd, buffer, MAX_BUFFER_SIZE);

    if (bytesRead > 0) {
        this->transactionMgr->appendToRequest(buffer, bytesRead); 
        if (this->transactionMgr->getRequestStatus() == FINISHED) { 
            this->state = WRITING_RESPONSE;
            this->action = MODIFY_TO_WRITE;
        }
        else if (this->transactionMgr->getRequestStatus() == ERROR) {
            this->state = ERROR_RESP;
            this->action = MODIFY_TO_WRITE;
        }
        delete[] buffer;
    }
    else {
        delete[] buffer;
        // i have to make sure what is the correct action here
    }
}

void ClientSocket::sendingResponse()
{
    int ret;
    std::pair<unsigned char *, size_t> response = this->transactionMgr->getResponse();
    if (response.second == 0) {
        if (this->transactionMgr->getResponseState() == WAITING_FOR_CGI) {
            return ;
        }
    }

    ret = write(socketFd, response.first, response.second);
    if (ret == -1) {
        this->action = CLOSE_CONNECTION;
        // im not sure
    }
    if (this->transactionMgr->getResponseState() == RESPONSE_FINISHED) {
        // if not keep-alive close the connection
        this->action = CLOSE_CONNECTION;
        // else i have to set action to MODIFY_TO_READ
    }
    delete[] response.first;
}

void ClientSocket::ErrorParseRequest() {
    int error_code = this->transactionMgr->getResponseCode();
    std::pair<unsigned char *, size_t> error_response = ErrorResponse::getErrorResponse(error_code);
    int ret = write(socketFd, error_response.first, error_response.second);
    delete[] error_response.first;
    if (ret == -1) {
        this->action = CLOSE_CONNECTION;
    }
    else {
        this->action = MODIFY_TO_READ;
    }
}

void ClientSocket::handleEvent()
{
	if (this->state == READING_REQUEST) {
        readingAndProcessingRequest();
    }
    else if (this->state == WRITING_RESPONSE) {
        sendingResponse();
    }
    else if (this->state == ERROR_RESP) {
        ErrorParseRequest();
    }
}