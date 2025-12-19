#include "HttpRequest.hpp"

HttpRequest ::HttpRequest(std ::string &_request) : request(_request)
{
    status = READING_REQUEST_LINE;
    response_code = 0;
    body_buffer_size = -1;
};

int HttpRequest::getBodyBufferSize() { return (this->body_buffer_size); }
enum RequestType HttpRequest::getType() { return (this->type); };
std::string HttpRequest::getUri() { return (this->uri); }
std::map<std::string, std::string> HttpRequest::getHeaders() { return (this->headers); }
std::string HttpRequest::getBody() { return (this->body); }
enum status HttpRequest::getStatus() { return (this->status); }
std::string &HttpRequest::getRequest() { return (this->request); }
int HttpRequest::getResponseCode() { return (this->response_code); }

void HttpRequest::SetBodyBufferSize(int bf_size) { this->body_buffer_size = bf_size; }
void HttpRequest::setStatus(enum status status) { this->status = status; }
void HttpRequest ::setBody(std ::string body) { this->body = body; }
void HttpRequest::setHeaders(std::map<std::string, std::string> headers) { this->headers = headers; }
void HttpRequest::setUri(std::string uri) { this->uri = uri; }
void HttpRequest::setType(enum RequestType type) { this->type = type; }
void HttpRequest::setResponseCode(int code) { this->response_code = code; }

std::string HttpRequest::getHeader(std::string key)
{
    std::map<std::string, std::string>::iterator it = this->headers.find(key);

    if (it != this->headers.end())
        return it->second;
    return "";
}

int HttpRequest::addHeader(std::string key, std::string value)
{
    this->headers[key] = value;
    return (1);
}
