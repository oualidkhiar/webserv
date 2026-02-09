#include "HttpRequest.hpp"
#include "RequestParser.hpp"
#include <algorithm>
#include "StringManip.hpp"

HttpRequest ::HttpRequest()
{
    status = READING_REQUEST_LINE;
    status_code = 200;
    reason_phrase = "OK";
    available_data = 0;
    location = NULL;
	cgi_type = NO_CGI;
}

int HttpRequest::getPort() { return (this->port); }
enum RequestType HttpRequest::getType() { return (this->type); };
std::string HttpRequest::getUri() { return (this->uri); }
enum HttpVersion HttpRequest::getHttpVersion() { return (this->http_version); }
std::map<std::string, std::string> HttpRequest::getHeaders() { return (this->headers); }
enum status HttpRequest::getStatus() { return (this->status); }
Body &HttpRequest::getBody() { return (this->body); }
size_t HttpRequest::getAvailableData() { return (this->available_data); }
struct serverConfig *HttpRequest::getConfig() { return (this->config); }
struct location *HttpRequest::getLocation() { return (this->location); }
std::vector<unsigned char> HttpRequest::getRequest() { return (this->request); };
unsigned char HttpRequest::getCharFromRequest(int index) { return (request.at(index)); }
size_t HttpRequest::requestSize() { return (request.size()); }

void HttpRequest::setPort(int port) { this->port = port; }
void HttpRequest::setConfig(struct serverConfig *config) { this->config = config; }
void HttpRequest::setAvailableData(size_t amount) { this->available_data += amount; };
void HttpRequest::setBody(Body &body) { this->body = body; }
void HttpRequest::setStatus(enum status status) { this->status = status; }
void HttpRequest::setHeaders(std::map<std::string, std::string> headers) { this->headers = headers; }
void HttpRequest::setType(enum RequestType type) { this->type = type; }
void HttpRequest::setCGIType(enum CGIType cgi_type) { this->cgi_type = cgi_type; }
void HttpRequest::setLocation(struct location *location) { this->location = location; }
void HttpRequest::clear() { request.clear(); }
void HttpRequest::setQuery(std::string query) {this->query_string = query;}
std::string HttpRequest::getQuery( void ) { return this->query_string;}
bool HttpRequest::hasError() { return (this->status == ERROR); }

void HttpRequest::checkCGI(const std::string &uri)
{
	size_t dot_pos = uri.find_last_of('.');
	if (dot_pos == std::string::npos)
	{
		this->setCGIType(NO_CGI);
		return;
	}

	std::string extension = uri.substr(dot_pos);

	if (this->location != NULL && !this->location->cgi_extension.empty())
	{
		for (size_t i = 0; i < this->location->cgi_extension.size(); i++)
		{
			if (this->location->cgi_extension[i] == extension)
			{
				if (extension == ".php")
					this->setCGIType(PHP_CGI);
				else if (extension == ".py")
					this->setCGIType(PYTHON_CGI);
				else if (extension == ".sh")
					this->setCGIType(SHELL_CGI);
				else
					this->setCGIType(NO_CGI);
				return;
			}
		}
	}
	this->setCGIType(NO_CGI);
}

bool HttpRequest::setUri(const std::string &uri)
{
	if (uri.empty())
	{
		this->setResponseCode(400, "Bad Request");
		return false;
	}

	bool is_origin = (uri[0] == '/');
	bool is_asterisk = (uri == "*");
	if (!is_origin && !is_asterisk)
	{
		this->setResponseCode(400, "Bad Request");
		return false;
	}
	checkCGI(uri);	
	this->uri = uri;
	return true;
}

void HttpRequest::setResponseCode(const int status_code, const std::string &reason_phrase)
{
	this->status_code = status_code;
	this->reason_phrase = reason_phrase;
	if (status_code > 299)
		this->status = ERROR;
}

// this function is added by saad
bool HttpRequest::setHttpVersion(const std::string &http_version)
{
	if (http_version == "HTTP/1.1")
	{
		this->http_version = HTTP_1_1;
		return true;
	}
	if (http_version == "HTTP/1.0")
	{
		this->http_version = HTTP_1_0;
		return true;
	}

	if (http_version.size() != 8 || http_version.compare(0, 5, "HTTP/") != 0 ||
	std::isdigit(http_version[5]) == false || http_version[6] != '.' ||
	std::isdigit(http_version[7]) == false)
	{
		this->setResponseCode(400, "Bad Request");
		return false;
	}
	else
	{
		this->setResponseCode(505, "HTTP Version Not Supported");
		return false;
	}
}

std::string HttpRequest::getHeader(std::string key)
{
    std::map<std::string, std::string>::iterator it = this->headers.find(key);

    if (it != this->headers.end())
        return it->second;
    return "";
}

void HttpRequest::appendRequestData(unsigned char *buffer, size_t buffer_size)
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
    std::vector<unsigned char>::iterator start_it;
    std::vector<unsigned char>::iterator end_it;
    start_it = request.begin() + start;
    end_it = start_it + len;
    request.erase(start_it, end_it);
}

std::string HttpRequest::extractString(size_t pos, size_t len)
{
    std::string extracted_string((char *)&request[pos], len);
    return (extracted_string);
}

std::vector<unsigned char> HttpRequest::getChunk(size_t start, size_t len)
{
    std::vector<unsigned char>::const_iterator first = request.begin() + start;
    std::vector<unsigned char>::const_iterator last = request.begin() + len;
    std::vector<unsigned char> chunk(first, last);
    return (chunk);
}

HttpRequest ::~HttpRequest()
{}

// debuging funcs

void HttpRequest::printHeaders()
{
    for (std::map<std::string, std::string>::iterator it = headers.begin(); it != headers.end(); ++it)
        std::cout << it->first << " : " << it->second << std::endl;
}

void HttpRequest::printBody()
{
    printVector(this->body.getBody());
}

void HttpRequest::printRequest()
{
    printVector(request);
}