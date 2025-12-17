#include "HttpRequest.hpp"

HttpRequest ::HttpRequest(std ::string &_request) : request(_request) {};
enum RequestType HttpRequest::getType() { return (this->type); };
std::string HttpRequest::getUri() { return (this->uri); }
std::map<std::string, std::string> HttpRequest::getHeaders() { return (this->headers); }
std::string HttpRequest::getBody() { return (this->body); }
enum status HttpRequest::getStatus() { return (this->status); }
std::string &HttpRequest::getRequest() { return (this->request); }

void HttpRequest::setStatus(enum status status) { this->status = status; }
void HttpRequest ::setBody(std ::string body) { this->body = body; }
void HttpRequest::setHeaders(std::map<std::string, std::string> headers) { this->headers = headers; }
void HttpRequest::setUri(std::string uri) { this->uri = uri; }
void HttpRequest::setType(enum RequestType type) { this->type = type; }

std::string HttpRequest::getHeader(std::string key)
{
    auto it = this->headers.find(key);
    if (it != headers.end())
        return (this->headers.find(key)->second);
    return ("NOT_FOUND");
}
