#pragma once
#define CHUNKED_HEADER "transfer-encoding"
#define FIXED_LENGTH_HEADER "content-length"
#define MAX_FILE_READ 8192
#define BUFFER_SIZE 8192
#define TIME_EXPIRATION 3600
#define COOKIES_PAGE 2222 

enum RequestType
{
    DELETE,
    POST,
    GET
};
enum status
{
    READING_REQUEST_LINE,
    READ_HEADER,
    READ_BODY,
    FINISHED,
    ERROR
};

enum ResponseState
{
    FRESH,
    READING_LARGE_FILE,
    WAITING_FOR_CGI,
    RESPONSE_FINISHED
};

enum ReadingType 
{
    CHUNKED,
    FIXED_LENGTH,
	EMPTY
};


enum CGIType
{
	NO_CGI,
	PHP_CGI,
	PYTHON_CGI,
	SHELL_CGI
};

enum HttpVersion
{
	HTTP_1_0,
	HTTP_1_1
};

enum ContentType
{
	NO_CT,
	MULTIPART_CT,
	APPLICATION_CT
};

enum MultipartState
{
	MP_READING_HEADERS,
	MP_WRITING_BODY,
	MP_COMPLETE
};

struct getInfos
{
    ResponseState state;
    int fd;
};


#define CONTENT_TYPE_HEADER "Content-Type"
#define PATH_DELIMITER "/"
#define DEFAULT_CONTENT_TYPE "application/octet-stream\r\n"

// 2xx Success
#define HP_OK                        200 // Successful GET request
#define HP_CREATED                   201 // POST: file successfully uploaded/created
#define HP_NO_CONTENT                204 // DELETE: file deleted successfully

// 3xx Redirection
#define HP_MOVED_PERMANENTLY         301 // 'return' directive in config
#define HP_FOUND                     302 // Temporary redirect

// 4xx Client Errors
#define HP_BAD_REQUEST               400 // Syntax error, missing Host header, chunk formatting error
#define HP_FORBIDDEN                 403 // chmod 000 file, or directory listing is OFF
#define HP_NOT_FOUND                 404 // Requested path does not exist
#define HP_METHOD_NOT_ALLOWED        405 // Method not in 'limit_except' block
#define HP_REQUEST_TIMEOUT           408
#define HP_LENGTH_REQUIRED           411 // POST with no Content-Length
#define HP_PAYLOAD_TOO_LARGE         413 // Body size > 'client_max_body_size'
#define HP_URI_TOO_LONG              414 // URI exceeds buffer size
#define HP_UNSUPPORTED_MEDIA_TYPE    415 // Content-Type not supported
#define HP_REQUEST_HEADER_TOO_LARGE  431 // Headers exceed buffer size

// 5xx Server Errors
#define HP_INTERNAL_SERVER_ERROR     500 // CGI crash or system call (read/write) failed
#define HP_NOT_IMPLEMENTED           501 // Method is not GET, POST, or DELETE
#define HP_BAD_GATEWAY               502 // CGI returned an invalid response
#define HP_SERVICE_UNAVAILABLE       503
#define HP_GATEWAY_TIMEOUT           504
#define HP_VERSION_NOT_SUPPORTED     505 // Request is not HTTP/1.1