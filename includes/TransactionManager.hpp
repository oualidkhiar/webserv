#pragma once
#include "HttpRequest.hpp"
#include "HttpResponse.hpp"
#include "enums.hpp"
#include "handleCgi.hpp"

class TransactionManager
{
private:
    std::string     listedFiles;
    HttpRequest     request;
    HttpResponse    response;
    Cgi             c;
    bool            responsed;

    void executeRequest();
    void readChunk();
    char *getRequestBuffer();
    size_t getRequestSize();
    std::pair<unsigned char *, size_t> getRoofResponse();
    std::pair<unsigned char *, size_t> joinPairs(std::pair<unsigned char *, size_t> &, std::pair<unsigned char *, size_t> &);
    std::pair<unsigned char *, size_t> firstResponse();

public:
    TransactionManager();
    void setServer(serverConfig *configg);
    RequestType getRequestType();
    status getRequestStatus();
    ResponseState getResponseState();
    void appendToRequest(unsigned char *buffer, size_t size);
    std::pair<unsigned char *, size_t> getResponse();
    int getResponseCode();
	serverConfig *getServer();
    // BOOL IS_KEEP_ALIVE
    // SET CONFIG
};