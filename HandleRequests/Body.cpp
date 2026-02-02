#include "Body.hpp"
#include "HttpRequest.hpp"
#include <cstring>

Body::Body() : to_read(0) {}

void Body::setToRead(size_t to_read) { this->to_read = to_read; }
void Body::setType(enum ReadingType type) { this->type = type; }
void Body::setBody(std::vector<unsigned char> chunk) { this->body = chunk; }

std::vector<unsigned char> Body::getBody() { return (this->body); }
enum ReadingType Body::getType() { return (this->type); }
size_t Body::getToRead() { return (this->to_read); }

enum ReadingType Body::discoverReadingType(HttpRequest &request)
{
    if (request.getHeader(CHUNKED_HEADER).empty() == false)
        setType(CHUNKED);
    else if (request.getHeader(FIXED_LENGTH_HEADER).empty() == false)
        setType(FIXED_LENGTH);
    return (type);
}

void Body::appendChunkToBody(std::vector<unsigned char> chunk)
{
    if (bodySize() == 0)
    {
        body = chunk;
        return;
    }
    body.insert(body.end(), chunk.begin(), chunk.end());
}

size_t Body::bodySize()
{
    return (this->body.size());
}

void Body::clearBody()
{
    this->body.clear();
}

unsigned char *Body::getCharVector()
{
    size_t chunkSize = bodySize();
    unsigned char *chunk = new unsigned char[chunkSize];
    std::memcpy(chunk, body.data(), chunkSize);
    return (chunk);
}

void Body::decrementToRead(size_t amount)
{
    if (amount >= to_read)
        to_read = 0;
    else
        to_read -= amount;
}

void Body::printBody()
{
    size_t i = 0;
    while (i < bodySize())
    {
        std::cout << body.at(i++);
    }
    std::cout << std::endl;
}

Body::~Body() {
}
