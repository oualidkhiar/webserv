#include "HttpRequest.hpp"
#include "RequestParser.hpp"
#include <algorithm>
#include "StringManip.hpp"

HttpRequest ::HttpRequest()
{
    status = READING_REQUEST_LINE;
    response_code = 0;
    body = NULL;
    available_data = 0;
    location = NULL;
}

int HttpRequest::getPort() { return (this->port); }
enum RequestType HttpRequest::getType() { return (this->type); };
std::string HttpRequest::getUri() { return (this->uri); }
std::map<std::string, std::string> HttpRequest::getHeaders() { return (this->headers); }
enum status HttpRequest::getStatus() { return (this->status); }
Body *HttpRequest::getBody() { return (this->body); }
size_t HttpRequest::getAvailableData() { return (this->available_data); }
struct serverConfig *HttpRequest::getConfig() { return (this->config); }
struct location *HttpRequest::getLocation() { return (this->location); }
std::vector<char> HttpRequest::getRequest() { return (this->request); };
char HttpRequest::getCharFromRequest(int index) { return (request.at(index)); }
size_t HttpRequest::requestSize() { return (request.size()); }

void HttpRequest::setPort(int port) { this->port = port; }
void HttpRequest::setConfig(struct serverConfig *config) { this->config = config; }
void HttpRequest::setAvailableData(size_t amount) { this->available_data += amount; };
void HttpRequest::setBody(Body *body) { this->body = body; }
void HttpRequest::setStatus(enum status status) { this->status = status; }
void HttpRequest::setHeaders(std::map<std::string, std::string> headers) { this->headers = headers; }
void HttpRequest::setUri(std::string uri) { this->uri = uri; }
void HttpRequest::setType(enum RequestType type) { this->type = type; }
void HttpRequest::setResponseCode(int code) { this->response_code = code; }
void HttpRequest::setLocation(struct location *location) { this->location = location; }
void HttpRequest::clear() { request.clear(); }

std::string HttpRequest::getHeader(std::string key)
{
    std::map<std::string, std::string>::iterator it = this->headers.find(key);

    if (it != this->headers.end())
        return it->second;
    return "";
}

void HttpRequest::appendRequestData(char *buffer, size_t buffer_size)
{
    RequestParser parser;
    this->request.insert(request.end(), buffer, buffer + buffer_size);
    parser.create_request(*this);
}

int HttpRequest::addHeader(std::string key, std::string value)
{
    this->headers[key] = value;
    return (1);
}

void HttpRequest::eraseFromRequest(size_t start, size_t len)
{
    std::vector<char>::iterator start_it;
    std::vector<char>::iterator end_it;
    start_it = request.begin() + start;
    end_it = start_it + len;
    request.erase(start_it, end_it);
}

std::string HttpRequest::extractString(size_t pos, size_t len)
{
    std::string extracted_string(&request[pos], len);
    return (extracted_string);
}

std::vector<char> HttpRequest::getChunk(size_t start, size_t len)
{
    std::vector<char>::const_iterator first = request.begin() + start;
    std::vector<char>::const_iterator last = request.begin() + len;
    std::vector<char> chunk(first, last);
    return (chunk);
}

HttpRequest ::~HttpRequest()
{
    if (body != NULL)
        delete body;
}

// debuging funcs

void HttpRequest::printHeaders()
{
    for (std::map<std::string, std::string>::iterator it = headers.begin(); it != headers.end(); ++it)
        std::cout << it->first << " : " << it->second << std::endl;
}

void HttpRequest::printBody()
{
    printVector(this->body->getBody());
}

void HttpRequest::printRequest()
{
    printVector(request);
}