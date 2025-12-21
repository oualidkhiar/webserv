#pragma once
#include <string>
#include "enums.hpp"
class HttpRequest;
class Body
{
private:
    std::string body;
    enum ReadingType type;
    size_t to_read;

public:
    Body();
    void setType(enum ReadingType type);
    void setToRead(size_t to_read);
    void setBody(std::string body);
    void SetToRead(HttpRequest & request);
    
    enum ReadingType discoverReadingType(HttpRequest & request);
    enum ReadingType getType();
    size_t getToRead();
    std::string getBody();
    ~Body();
};