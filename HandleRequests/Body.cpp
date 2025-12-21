#include "Body.hpp"
#include "HttpRequest.hpp"
Body::Body() : to_read(0) {}

void Body::setToRead(size_t to_read) { this->to_read = to_read; }
void Body::setType(enum ReadingType type) { this->type = type; }
void Body::setBody(std::string body) { this->body = body; }

std::string Body::getBody() { return (this->body); }
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

Body::~Body() {}
