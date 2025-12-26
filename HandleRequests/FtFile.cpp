#include "FtFile.hpp"

FtFile::FtFile(const std::string &path)
    : fd(-1),
      sented_bytes(0),
      path(path),
      state(FileReadState::NOT_OPENED) {}

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


FtFile::~FtFile(){}