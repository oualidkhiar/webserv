#pragma once
#include "HttpRequest.hpp"
#include "HttpResponse.hpp"
#include "enums.hpp"

class TransactionManager
{
private:
    HttpRequest request;
    HttpResponse response;
    void executeRequest();
    void readChunk();
    char * getRequestBuffer();
    size_t getRequestSize();
public:
    
    TransactionManager();
    void setServer(serverConfig * configg);
    RequestType getRequestType();
    status getRequestStatus();
    ResponseState getResponseState();
    void appendToRequest(unsigned char *buffer, size_t size);
    std::pair<unsigned char *, size_t> getResponse();
    // BOOL IS_KEEP_ALIVE
    // SET CONFIG
    
};