#include "clientSocket.hpp"
#include "server_manager.hpp"
#include "ErrorResponse.hpp"
#include "constent.hpp"

ClientSocket::ClientSocket(int fd ,serverConfig *conf): 
socketsManager(conf, fd), state(READING_REQUEST)
{
    this->transactionMgr = new TransactionManager();
    this->transactionMgr->setServer(conf);
}

bool ClientSocket::isTimeOut() {
    return time(NULL) - this->lastTimeInteraction >= MAX_TIME_WITHOUT_INTERACTION;
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
            DisplyLogs::printCurrentAtion("[INFO ] [MODIFY]", "client finished processing request ready to write response", GREEN);
        }
        else if (this->transactionMgr->getRequestStatus() == ERROR) {
            this->state = ERROR_RESP;
            this->action = MODIFY_TO_WRITE;
            DisplyLogs::printCurrentAtion("[ERROR] [REQ  ]", "request error parse", RED);
        }
        else {
            std::ostringstream s;
            s << bytesRead;
            DisplyLogs::printCurrentAtion("[INFO ] [REQ  ]", "reading "+s.str()+" bytes from request", GREEN);
        }
    }
    else {
        if (bytesRead == 0) {
            DisplyLogs::printCurrentAtion("[WRNIN] [REQ  ]", "client close connection", YELLOW);
            this->action = CLOSE_CONNECTION;
        }
        else {
            DisplyLogs::printCurrentAtion("[ERROR] [REQ  ]", "syscall read failed", RED);
            this->action = CLOSE_CONNECTION;
        }
    }
    delete[] buffer;
}

void ClientSocket::sendingResponse()
{
    int ret;
    std::pair<unsigned char *, size_t> response = this->transactionMgr->getResponse();
    if (response.second == 0) {
        if (this->transactionMgr->getResponseState() == WAITING_FOR_CGI) {
            DisplyLogs::printCurrentAtion("[INFO ] [RESP ]", "waiting for CGI to finish execution...", GREEN);
            return ;
        }
    }
    ret = write(socketFd, response.first, response.second);
    if (ret == -1) { 
        DisplyLogs::printCurrentAtion("[ERROR] [RESP ]", "write() failed: connection broken or socket closed", RED);
        this->action = CLOSE_CONNECTION;
    }
    else if (this->transactionMgr->getResponseState() == RESPONSE_FINISHED) {
        DisplyLogs::printCurrentAtion("[INFO ] [RESP ]", "Response fully sent to client", GREEN);
        this->action = CLOSE_CONNECTION;
    }
    delete[] response.first;
}

void ClientSocket::ErrorParseRequest() {
    int ret;
    int error_code = this->transactionMgr->getResponseCode();
    std::pair<unsigned char *, size_t> error_response = ErrorResponse::getErrorResponse(serverConf, error_code);
    ret = write(socketFd, error_response.first, error_response.second);
    if (ret == -1) {
        this->action = CLOSE_CONNECTION;
        DisplyLogs::printCurrentAtion("[ERROR] [RESP ]", "write() failed: connection broken or socket closed", RED);
    }
    else {
        DisplyLogs::printCurrentAtion("[INFO ] [RESP ]", "Error response successfully delivered to client", GREEN);
        this->action = CLOSE_CONNECTION;
    }
    delete[] error_response.first;
}

void ClientSocket::handleEvent()
{
	if (this->state == READING_REQUEST) {
        readingAndProcessingRequest();
    }
    else if (this->state == WRITING_RESPONSE) {
        sendingResponse();
    }
    else if (this->state == ERROR_RESP) { // error response that we detect at parsing time are serving using this function
                                        // but if we detect error at execution time we hanle it in transaction manager class 
        ErrorParseRequest();
    }
}