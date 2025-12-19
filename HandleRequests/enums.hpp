#pragma once
#define CHUNKED "Transfer-Encoding"
#define FIXED_LENGTH "Content-Length"

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
