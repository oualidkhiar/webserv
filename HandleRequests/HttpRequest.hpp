#pragma once
#include <string>
#include <map>
#include "enums.hpp"
#include "Body.hpp"
#include "../includes/config.hpp"

class HttpRequest

{
private:
    enum RequestType type;
    std::string uri;
    std::string query_string;
    std::map<std::string, std::string> headers;
    enum status status;
    int response_code;
    Body *body;
    size_t available_data;
    struct serverConfig *config;
    int port;
    struct location *location;
    std::vector<unsigned char> request;

public:
    HttpRequest();
    enum RequestType getType();
    std::string getUri();
    std::map<std::string, std::string> getHeaders();
    std::string getHeader(std::string key);
    enum status getStatus();
    int getResponseCode();
    Body *getBody();
    int addHeader(std::string key, std::string value);
    size_t getAvailableData();
    struct serverConfig *getConfig();
    int getPort();
    struct location *getLocation();
    std::vector<unsigned char> getRequest();
    void appendRequestData(unsigned char *buffer, size_t buffer_size);
    unsigned char getCharFromRequest(int index);
    size_t requestSize();
    std::string getQuery( void );

    void eraseFromRequest(size_t start, size_t len);
    std::string extractString(size_t pos, size_t len);
    std::vector<unsigned char> getChunk(size_t pos, size_t len);

    void clear();
    void setLocation(struct location *location);
    void setPort(int port);
    void setConfig(struct serverConfig *config);
    void setAvailableData(size_t available_data);
    void setBody(Body *body);
    void setResponseCode(int code);
    void setHeaders(std::map<std::string, std::string> headers);
    void setUri(std::string uri);
    void setType(enum RequestType type);
    void setStatus(enum status status);
    void setQuery(std::string query);
    ~HttpRequest();

    // DEBUGING FINCTIONS
    void printHeaders();
    void printBody();
    void printRequest();
};