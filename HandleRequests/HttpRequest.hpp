#pragma once
#include <string>
#include <map>
#include "enums.hpp"
#include "Body.hpp"
#include "../includes/config.hpp"
#include "FtFile.hpp"

class HttpResponse;

class HttpRequest
{
private:
    std::vector<unsigned char> request;

    enum RequestType type;
    std::string uri;
    std::string query_string;
	enum HttpVersion http_version;

    std::map<std::string, std::string> headers;
    Body body;
	enum CGIType cgi_type;
    int status_code;
	std::string	reason_phrase;

    enum status status;
    size_t available_data;
    struct serverConfig *config;
    int port;
    struct location *location;
	FtFile *file;
	ContentType CT;

public:
    HttpRequest();
    enum CGIType getCGIType();
    void setCGIType(enum CGIType cgi_type);
    enum RequestType getType();
    std::string getUri();
    enum HttpVersion getHttpVersion();
    std::map<std::string, std::string> getHeaders();
    std::string getHeader(std::string key);
    enum status getStatus();
    int getResponseCode();
    Body &getBody();
    int addHeader(std::string key, std::string value);
    size_t getAvailableData();
    struct serverConfig *getConfig();
    int getPort();
    struct location *getLocation();
    std::vector<unsigned char> getRequest();
    void appendRequestData(unsigned char *buffer, size_t buffer_size, HttpResponse &response);
    unsigned char getCharFromRequest(int index);
    size_t requestSize();
    std::string getQuery( void );
    void checkCGI(const std::string &uri);
    void eraseFromRequest(size_t start, size_t len);
    std::string extractString(size_t pos, size_t len);
    std::vector<unsigned char> getChunk(size_t pos, size_t len);
	FtFile *getFtFile(void);
	ContentType	getContentType(void);

    void clear();
	void setContentType(ContentType CT);
	void setFtFile(FtFile *file);
    void setLocation(struct location *location);
    void setPort(int port);
    void setConfig(struct serverConfig *config);
    void setAvailableData(size_t available_data);
    void setBody(Body &body);
    void setResponseCode(const int status_code, const std::string &reason_phrase);
    void setHeaders(std::map<std::string, std::string> headers);
    bool setUri(const std::string &uri);
    void setType(enum RequestType type);
    void setStatus(enum status status);
    void setQuery(std::string query);
	bool setHttpVersion(const std::string &http_version);
	bool hasError();
    ~HttpRequest();

    // DEBUGING FINCTIONS
    void printHeaders();
    void printBody();
    void printRequest();
	void setRequest(std::vector<unsigned char> request);
};