#pragma once
#include "HttpRequest.hpp"
#include "HttpResponse.hpp"
#include "../includes/config.hpp"
#include "handleCgi.hpp"

enum ExecutorCase{
    CGI_EXECUTION,
    NONE
};

class Executor
{
private:
    ExecutorCase Case;
    // methods // 
    void setContentTpe(HttpResponse & response , const std::string &path);
    std::pair<int, std::string> pathResolver(HttpRequest & request);
    bool isAllowedMethod(HttpRequest &request);
    void setLocation(HttpRequest &request);
    int matchedScore(std::string uri, std::string key);
    void executeDelete(HttpRequest &request, HttpResponse & response);
    void executePost(HttpRequest &request, HttpResponse & response);
    location *getLongestMatchedLocation(HttpRequest &request, std::map<std::string, location *> map);
    void executeGet(HttpRequest &request, HttpResponse & response);
    std::pair<int , FtFile *> extractFileInfos(const char * path);
    
    // Multipart parsing helpers
    std::string extractBoundary(const std::string &contentType);
    void parseMultipartBody(HttpRequest &request, HttpResponse &response, const std::string &boundary);
    std::string extractHeaderValue(const std::string &headers, const std::string &key);
    bool saveUploadedFile(const std::string &uploadDir, const std::string &filename, 
                          const std::vector<unsigned char> &content);
    std::string pathResolverForDelete(HttpRequest &request);
    // 
    std::pair<int, FtFile *> getIndexFile(HttpRequest& request);
    void caseRedirection(HttpResponse& response, std::string& path, int code);
    void caseIndexFile(HttpResponse& resp, HttpRequest& req);
    void caseListingFiles(HttpResponse& resp, HttpRequest& req, std::string& path);
    void caseSpecifiedFile(HttpResponse& response, std::string& path);
    void caseForbiden(HttpResponse& resp);

    public:
    Executor();
    ExecutorCase getExecutorCase();
    void execute(HttpRequest &request , HttpResponse & response);
};

