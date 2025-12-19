#include "HttpResponse.hpp"

HttpResponse::HttpResponse() {}
void HttpResponse::setStatus(int status) { this->status = status; }
int HttpResponse::getStatus() { return (this->status); }
std::string HttpResponse::getBody() { return this->body; }
void HttpResponse::setBody(std::string body) { this->body = body; }
void HttpResponse::AddHeader(std::string key, std::string value) { this->headers[key] = value; }

std::string HttpResponse::getHeader(std::string key)
{
    std::map<std::string, std::string>::iterator it = this->headers.find(key);

    if (it != this->headers.end())
        return it->second;
    return "NOT_FOUND";
}
HttpResponse::~HttpResponse() {}