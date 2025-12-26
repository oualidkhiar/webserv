#include "Body.hpp"
#include "HttpRequest.hpp"
Body::Body() : to_read(0) {}

void Body::setToRead(size_t to_read) { this->to_read = to_read; }
void Body::setType(enum ReadingType type) { this->type = type; }
void Body::setBody(std::vector<char> chunk) { this->body = chunk; }

std::vector<char> Body::getBody() { return (this->body); }
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

void Body::appendChunkToBody(std::vector<char> chunk)
{
    body.insert(body.end(), chunk.begin(), chunk.end());
}

void Body::decrementToRead(size_t amount)
{
    if (amount >= to_read)
        to_read = 0;
    else
        to_read -= amount;
}

Body::~Body() {}
