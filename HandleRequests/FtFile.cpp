#include "FtFile.hpp"

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


std::vector<unsigned char> FtFile::readFile()
{
    
}

FtFile::~FtFile() {}