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



std::string HttpResponse::getReasonPhrase(int code)
{
    switch (code)
    {
        // --- 2xx Success ---
        case HP_OK:
            return "OK";
        case HP_CREATED:
            return "Created";
        case HP_NO_CONTENT:
            return "No Content";

        // --- 3xx Redirection ---
        case HP_MOVED_PERMANENTLY:
            return "Moved Permanently";

        // --- 4xx Client Error ---
        case HP_BAD_REQUEST:
            return "Bad Request";
        case HP_FORBIDDEN:
            return "Forbidden";
        case HP_NOT_FOUND:
            return "Not Found";
        case HP_METHOD_NOT_ALLOWED:
            return "Method Not Allowed";
        case HP_PAYLOAD_TOO_LARGE:
            return "Payload Too Large"; // Note: RFC 7231 changed this from "Request Entity Too Large"

        // --- 5xx Server Error ---
        case HP_INTERNAL_SERVER_ERROR:
            return "Internal Server Error";
        case HP_NOT_IMPLEMENTED:
            return "Not Implemented";
        case HP_VERSION_NOT_SUPPORTED:
            return "HTTP Version Not Supported";

        // --- Default / Fallback ---
        default:
            return "Unknown Status";
    }
}

// HttpResponse HttpResponse::getResponse(HttpRequest & request)
// {
//     HttpResponse response;
//     // create response
    
    
    
//     //return response
//     response.AddHeader(FIXED_LENGTH_HEADER , std::to_string(response.getBody().size()));
//     return response;
// }

HttpResponse::~HttpResponse() {}
