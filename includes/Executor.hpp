#pragma once
#include "HttpRequest.hpp"
#include "HttpResponse.hpp"
#include "config.hpp"
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
    void executeDelete(HttpRequest &request, HttpResponse & response);
    void executeGet(HttpRequest &request, HttpResponse & response);
    std::pair<int , FtFile *> extractFileInfos(const char * path);

    void CookiesHandler(HttpResponse &response);
    void CookiesForm(HttpResponse &response);
    void CookiesWelcomePage(HttpResponse &response);

    // Multipart parsing helpers
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
	static bool isAllowedMethod(HttpRequest &request);
	static void setLocation(HttpRequest &request);
	static int matchedScore(std::string uri, std::string key);
	static    location *getLongestMatchedLocation(HttpRequest &request, std::map<std::string, location *> map);
	

};

