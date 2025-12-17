#include <HttpResponse.hpp>

HttpResponse::HttpResponse() {}
void HttpResponse::setStatus(int status) { this->status = status; }
int HttpResponse::getStatus() { return (this->status); }
std::string HttpResponse::getBody() { return this->body; }
void HttpResponse::setBody(std::string body) { this->body = body; }
int HttpResponse::AddHeader(std::string key, std::string value) { this->headers.insert(key, value); }

std::string HttpResponse::getHeader(std::string key)
{
    auto it = this->headers.find(key);
    if (it != headers.end())
        return (this->headers.find(key)->second);
    return ("NOT_FOUND");
}
HttpResponse::~HttpResponse() {}