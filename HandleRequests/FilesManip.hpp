#pragma once
#include <string>


class FilesManip
{
public:
    std::string readFile(int fd, size_t size);
};