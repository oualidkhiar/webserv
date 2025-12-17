#pragma once
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
    FINISHED
};
