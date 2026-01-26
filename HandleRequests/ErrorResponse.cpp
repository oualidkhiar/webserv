// #pragma once
#include <string>
class ErrorResponse
{
public:
    std::pair<unsigned char *, size_t> getErrorResponse(int code);
};