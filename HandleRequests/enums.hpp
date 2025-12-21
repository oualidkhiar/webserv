#pragma once
#define CHUNKED_HEADER "Transfer-Encoding"
#define FIXED_LENGTH_HEADER "Content-Length"

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

enum ReadingType
{
    CHUNKED,
    FIXED_LENGTH
};