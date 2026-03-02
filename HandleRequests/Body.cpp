#include "Body.hpp"
#include "HttpRequest.hpp"
#include <cctype>
#include <cstring>

Body::Body() : type(EMPTY), to_read(0) {}

void Body::setToRead(size_t to_read) { this->to_read = to_read; }
void Body::setBody(std::vector<unsigned char> chunk) { this->body = chunk; }
void Body::setType(enum ReadingType type) { this->type = type;}

const std::vector<unsigned char>& Body::getBody() { return (this->body); }
enum ReadingType Body::getType() { return (this->type); }
size_t Body::getToRead() { return (this->to_read); }

enum ReadingType Body::discoverReadingType(HttpRequest &request) 
{
    std::string te = request.getHeader(CHUNKED_HEADER);
    std::string te_lower = te;
    for (size_t i = 0; i < te_lower.size(); i++)
        te_lower[i] = std::tolower(te_lower[i]);
    bool has_chunked = (te_lower.find("chunked") != std::string::npos);
    bool has_content_length = !request.getHeader(FIXED_LENGTH_HEADER).empty();
    
    //if both are present, return EMPTY (security: request smuggling)
    if (has_chunked && has_content_length)
    {
        request.setResponseCode(400, "Bad Request");
        this->type = EMPTY;
        return EMPTY;
    }
    
    if (has_chunked)
    {
        setType(CHUNKED);
        return CHUNKED;
    }
    if (has_content_length)
	{
		setType(FIXED_LENGTH);
		return FIXED_LENGTH;
	}
    
    this->type = EMPTY;
    return EMPTY;  //no body is present
}


// enum ReadingType Body::discoverReadingType(HttpRequest &request)
// {
//     if (request.getHeader(CHUNKED_HEADER).empty() == false)
//         setType(CHUNKED);
//     else if (request.getHeader(FIXED_LENGTH_HEADER).empty() == false)
//         setType(FIXED_LENGTH);
//     return (type);
// }

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

Body::~Body() {}
