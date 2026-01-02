#pragma once
#include <vector>
#include "enums.hpp"
#include <map>
#include <string>
#include "FtFile.hpp"
#include "HttpRequest.hpp"
#include "Body.hpp"

class HttpResponse
{
private:
    std::string header;
    RequestType type;
    int status;
    std::map<std::string, std::string> headers;
    Body *body;
    ResponseState state;
    FtFile *file;
    void createHeaderLine();
    void createHeaders();

public:
    HttpResponse();

    void setStatus(int status);
    void AddHeader(std ::string key, std::string value);
    void setFile(FtFile *file);
    void setState(ResponseState state);
    void createBody();
    void setBody(Body *body);
    void initializeResponse();
    std::string getHeaderLine();
    size_t bodySize();
    void appendBodyToResponse(std::vector<unsigned char> &chunk);
    std::pair<unsigned char *, size_t> getChunkFromRequest();
    ResponseState getState();
    FtFile *getFile();
    std::string getReasonPhrase(int code);
    int getStatus();
    std::string getHeader(std::string key);
    static HttpResponse getResponse(HttpRequest &request);
    ~HttpResponse();
    void printHeaders();

    void printBody();
};