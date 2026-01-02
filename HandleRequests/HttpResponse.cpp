#include "HttpResponse.hpp"
#include <sstream>

HttpResponse::HttpResponse()
{
    this->state = FRESH;
    file = NULL;
    body = NULL;
}

void HttpResponse::setState(ResponseState state) { this->state = state; }
std::string HttpResponse::getHeaderLine() { return (this->header); }
ResponseState HttpResponse::getState() { return this->state; }
int HttpResponse::getStatus() { return (this->status); }
void HttpResponse::AddHeader(std::string key, std::string value) { this->headers[key] = value; }

void HttpResponse::setFile(FtFile *file)
{
    this->file = file;
}

void HttpResponse::createHeaderLine()
{
    std::ostringstream os;
    os << this->status;
    this->header = "HTTP/1.1 " + os.str() + " " + getReasonPhrase(this->status) + "\r\n";
}

FtFile *HttpResponse::getFile()
{
    return file;
}

void HttpResponse::setStatus(int status)
{
    this->status = status;
    createHeaderLine();
}

void HttpResponse::createHeaders()
{
    std::ostringstream os;
    os << this->file->getFileSize();
    this->headers.insert(std::make_pair("Connection", "Closed\r\n"));
    this->headers.insert(std::make_pair("server", "TestServer/1.1\r\n"));
    this->headers.insert(std::make_pair(FIXED_LENGTH_HEADER, os.str() + "\r\n"));
}

std::pair<unsigned char *, size_t> HttpResponse::getChunkFromRequest()
{
    size_t chunkSize = bodySize();
    unsigned char *chunk = body->getCharVector();
    this->body->clearBody();
    return (std::make_pair(chunk, chunkSize));
}

size_t HttpResponse::bodySize()
{
    return (this->body->bodySize());
}

void HttpResponse::initializeResponse()
{
    createHeaderLine();
    createHeaders();
}

std::string HttpResponse::getHeader(std::string key)
{
    std::map<std::string, std::string>::iterator it = this->headers.find(key);

    if (it != this->headers.end())
        return it->second;
    return "NOT_FOUND";
}

void HttpResponse::appendBodyToResponse(std::vector<unsigned char> &chunk)
{
    this->body->appendChunkToBody(chunk);
}

//------------ debug---------------
void HttpResponse::printBody()
{
    this->body->printBody();
}

void HttpResponse::printHeaders()
{
    for (std::map<std::string, std::string>::const_iterator it = headers.begin(); it != headers.end(); ++it)
        std::cout << it->first << " : " << it->second << std::endl;
}
//---------------------------------

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

void HttpResponse::createBody()
{
    Body *body = new Body();
    this->body = body;
}

void HttpResponse::setBody(Body *body)
{
    this->body = body;
}

std::map<std::string, std::string>::const_iterator HttpResponse::headersBegin()
{
    return (this->headers.begin());
}
std::map<std::string, std::string>::const_iterator HttpResponse::headersEnd()
{
    return (this->headers.end());
}

HttpResponse::~HttpResponse()
{
    if (file != NULL)
    {
        delete file;
    }
    if (body != NULL)
    {
        delete body;
    }
}
