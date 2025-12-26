#pragma once
#include <string>
#include "enums.hpp"
class HttpRequest;
class Body
{
private:
    std::vector<char> body;
    enum ReadingType type;
    size_t to_read;

public:
    Body();
    void setType(enum ReadingType type);
    void setToRead(size_t to_read);
    void setBody(std::vector<char> chunk);
    void decrementToRead(size_t amount);
    void appendChunkToBody(std::vector<char> chunk);

    enum ReadingType discoverReadingType(HttpRequest &request);
    enum ReadingType getType();
    size_t getToRead();
    std::vector<char> getBody();
    ~Body();
};