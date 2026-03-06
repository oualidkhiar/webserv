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
        if (errno == EAGAIN or errno == EWOULDBLOCK)
            DisplyLogs::printCurrentAtion("[WARNIN] [REQ  ]", "read would block, no data available right now, try again later", YELLOW);
        else if (errno != EINTR) {
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
        if (errno == EPIPE or errno == ECONNRESET) {
            DisplyLogs::printCurrentAtion("[INFO ] [RESP ]", "client close connection", GREEN);
            this->action = CLOSE_CONNECTION;
        }
        else if (errno == EAGAIN) {
            DisplyLogs::printCurrentAtion("[WRNING] [RESP ]", "write would block — send buffer full, try again later", YELLOW);
        }
    }
    if (this->transactionMgr->getResponseState() == RESPONSE_FINISHED) {
        DisplyLogs::printCurrentAtion("[INFO ] [RESP ]", "Response (HTTP 200) fully sent to client", GREEN);
        this->action = CLOSE_CONNECTION;
    }
    delete[] response.first;
}

void ClientSocket::ErrorParseRequest() {
    int ret;
    int error_code = this->transactionMgr->getResponseCode();
    std::pair<unsigned char *, size_t> error_response = ErrorResponse::getErrorResponse(error_code);
    ret = write(socketFd, error_response.first, error_response.second);
    if (ret == -1) {
        if (errno == EINTR or errno == EWOULDBLOCK or errno == EAGAIN) {
            DisplyLogs::printCurrentAtion("[WRNIN] [RESP ]", "read syscall temporarily unavailable (EAGAIN/EINTR)", YELLOW);
            delete[] error_response.first;
            return ;
        }
        else {
            this->action = CLOSE_CONNECTION;
            delete[] error_response.first;
            DisplyLogs::printCurrentAtion("[ERROR] [RESP ]", "write syscall failed", RED);
        }
    }
    else {
        delete[] error_response.first;
        DisplyLogs::printCurrentAtion("[INFO ] [RESP ]", "Error response successfully delivered to client", GREEN);
        this->action = CLOSE_CONNECTION;
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