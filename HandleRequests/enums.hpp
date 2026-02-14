#pragma once
#define CHUNKED_HEADER "transfer-encoding"
#define FIXED_LENGTH_HEADER "content-length"
#define MAX_FILE_READ 8192
#define BUFFER_SIZE 8192

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

struct getInfos 
{
    ResponseState state;
    int fd;
    

};


#define CONTENT_TYPE_HEADER "Content-Type"
#define PATH_DELIMITER "/"
#define DEFAULT_CONTENT_TYPE "application/octet-stream\r\n"

                                 
#define HP_OK               200  // Standard response for successful GET requests
#define HP_CREATED          201  // Mandatory for POST: When a file is successfully uploaded/created
#define HP_NO_CONTENT       204  // Mandatory for DELETE: When a file is deleted successfully

                              
#define HP_MOVED_PERMANENTLY 301 // Used for 'return' directive in config (e.g., redirect to https)

                                
#define HP_BAD_REQUEST       400 // Parser Error: Syntax error, missing Host header, or chunk formatting error
#define HP_FORBIDDEN         403 // Permissions Error: 'chmod 000' file or directory listing is OFF
#define HP_NOT_FOUND         404 // Router Error: The requested file path does not exist
#define HP_METHOD_NOT_ALLOWED 405 // Config Limit: Request method not in 'limit_except' block
#define HP_PAYLOAD_TOO_LARGE 413 // Config Limit: Body size > 'client_max_body_size'

                              
#define HP_INTERNAL_SERVER_ERROR 500 // CGI Error: Script crashed or system call (read/write) failed
#define HP_NOT_IMPLEMENTED       501 // Parser Error: Method is not GET, POST, or DELETE
#define HP_VERSION_NOT_SUPPORTED 505 // Parser Error: Request is not HTTP/1.1