#include "FtFile.hpp"
#include "enums.hpp"
#include <unistd.h>
#include <fcntl.h>
#include "StringManip.hpp"
#include <iostream>

FtFile::FtFile(const std::string &path)

{
    state = NOT_OPENED;
    remove_file = false;
    sented_bytes = 0;
    fd = -1;
    this->path = path;
    file_size = 0;
}

void FtFile::setRemoveFile(bool t_f) {
    this->remove_file = t_f;
}

bool FtFile::shouldRemovingFile() {
    return this->remove_file;
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
    if (fd != -1){
        close(fd);
        fd = -1;
    }
    if (this->remove_file) {
        std::remove(this->path.c_str());
    }
    this->state = FILE_FINISHED;
}

std::vector<unsigned char> FtFile::readFile()
{
    std::vector<unsigned char> chunk;
    unsigned char buffer[MAX_FILE_READ];
    int bytes_read;
    if (fd == -1)
    {
        std::cout << "\nft_file readFile opened " << path << std::endl;
        fd = open(path.c_str(), O_RDONLY);
        this->state = FILE_READING;
        if (fd < 0)
        {
            // handle failair
            exit_error("POPEN FAILED");
        }
    }
    bytes_read = read(fd, buffer, MAX_FILE_READ);
    if (bytes_read < MAX_FILE_READ)
    {
        ft_close();
    }
    if (bytes_read > 0)
        chunk.assign(buffer, buffer + bytes_read);
    return (chunk);
}

int FtFile::writeToFile(const std::vector<unsigned char> &data, bool close)
{
    int ret;
    if (fd == -1)
    {
        fd = open(path.c_str(), O_CREAT | O_WRONLY, 0644); // modified. old version :  fd = open(path.c_str(), O_WRONLY | O_CREAT | O_TRUNC, 0644);
		if (fd == -1)
			return -1;
        this->state = FILE_WRITING;
    }
    if (close)
    {
        ret = write(fd, &data[0], data.size());
        if (ret < 0) { // write fail before writing all the data
            if (errno == EINTR) { // if the cause is interupt try call write another time
                ret = write(fd, &data[0], data.size());        
            }
            if (ret < 0) {
                this->setRemoveFile(true);
                ft_close();
                return -1;
            }
        }
        ft_close();
        return 1;
    }
    ret = write(fd, &data[0], data.size());
    if (ret < 0) { // write fail before writing all the data
        if (errno == EINTR) { // if the cause is interupt try call write another time 
            ret = write(fd, &data[0], data.size());        
        }
        if (ret < 0) {
            this->setRemoveFile(true);
            ft_close();
            return -1;
        }
    }
    return 1;
}

FtFile::~FtFile() {}