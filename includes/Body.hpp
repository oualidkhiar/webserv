#pragma once
#include <string>
#include "enums.hpp"
#include <vector>
class HttpRequest;
class Body
{
private:
    std::vector<unsigned char> body;
    enum ReadingType type;
    size_t to_read;

public:
    Body();
    void setType(enum ReadingType type);
    void setToRead(size_t to_read);
    void setBody(std::vector<unsigned char> chunk);
    void decrementToRead(size_t amount);
    void appendChunkToBody(std::vector<unsigned char> chunk);
    void clearBody();
	void printBody();
    unsigned char * getCharVector();
    size_t bodySize();
    enum ReadingType discoverReadingType(HttpRequest &request);
    enum ReadingType getType();
    size_t getToRead();
    const std::vector<unsigned char>& getBody();
    ~Body();
};