#include "HttpRequest.hpp"

HttpRequest ::HttpRequest(std ::string &_request) : request(_request)
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
std::string &HttpRequest::getRequest() { return (this->request); }
int HttpRequest::getResponseCode() { return (this->response_code); }
Body *HttpRequest::getBody() { return (this->body); }
size_t HttpRequest::getAvailableData() { return (this->available_data); }
struct serverConfig *HttpRequest::getConfig() { return (this->config); }
struct location * HttpRequest::getLocation(){return (this->location);}

void HttpRequest::setPort(int port) { this->port = port; }
void HttpRequest::setConfig(struct serverConfig *config) { this->config = config; }
void HttpRequest::setAvailableData(size_t amount) { this->available_data += amount; };
void HttpRequest::setBody(Body *body) { this->body = body; }
void HttpRequest::setStatus(enum status status) { this->status = status; }
void HttpRequest::setHeaders(std::map<std::string, std::string> headers) { this->headers = headers; }
void HttpRequest::setUri(std::string uri) { this->uri = uri; }
void HttpRequest::setType(enum RequestType type) { this->type = type; }
void HttpRequest::setResponseCode(int code) { this->response_code = code; }
void HttpRequest::setLocation(struct location * location) {this->location = location;}

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

HttpRequest ::~HttpRequest()
{
    if (body != NULL)
        delete body;
}