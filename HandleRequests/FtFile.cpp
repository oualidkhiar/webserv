#include "FtFile.hpp"
#include "enums.hpp"
#include <unistd.h>
#include <fcntl.h>
#include "StringManip.hpp"
#include <iostream>

FtFile::FtFile(const std::string &path)

{
    state = NOT_OPENED;
    sented_bytes = 0;
    fd = -1;
    this->path = path;
    file_size = 0;
}

int FtFile::getFd() const
{
    return fd;
}

size_t FtFile::getSentedBytes() const
{
    return sented_bytes;
}

const std::string &FtFile::getPath() const
{
    return path;
}

FileReadState FtFile::getState() const
{
    return state;
}

size_t FtFile::getFileSize()
{
    return (file_size);
}

void FtFile::setFileSize(size_t size)
{
    file_size = size;
}

void FtFile::setFd(int newFd)
{
    fd = newFd;
}

void FtFile::setSentedBytes(size_t bytes)
{
    sented_bytes = bytes;
}

void FtFile::setPath(const std::string &newPath)
{
    path = newPath;
}

void FtFile::setState(FileReadState newState)
{
    state = newState;
}

void FtFile::incrementSentedBytes(size_t amount)
{
    sented_bytes += amount;
}

void FtFile::ft_close()
{
    close(fd);
    this->state = FILE_FINISHED;
}

std::vector<unsigned char> FtFile::readFile()
{
    std::vector<unsigned char> chunk;
    unsigned char buffer[MAX_FILE_READ];
    int bytes_read;
    if (fd == -1)
    {
        fd = open(path.c_str(), O_RDONLY);
        if (fd < 0)
        {
            // handle failair
            exit_error("POPEN FAILED");
        }
    }
    bytes_read = read(fd, buffer, MAX_FILE_READ);
    if (bytes_read < MAX_FILE_READ)
        ft_close();
    if (bytes_read > 0)
        chunk.assign(buffer, buffer + bytes_read);
    return (chunk);
}

FtFile::~FtFile() {}