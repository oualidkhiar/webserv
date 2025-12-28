#pragma once
#include <string>
#include <vector>

enum FileReadState
{
    NOT_OPENED,
    FILE_READING,
    FILE_FINISHED
};

class FtFile
{
private:
    int fd;
    size_t sented_bytes;
    std::string path;
    FileReadState state;

public:
    FtFile(const std::string &path);

    // Getters
    int getFd() const;
    size_t getSentedBytes() const;
    const std::string &getPath() const;
    FileReadState getState() const;
    void incrementSentedBytes(size_t amount);
    std::vector<char> getChunk(size_t macChunkSize);


    // Setters
    void setFd(int newFd);
    void setSentedBytes(size_t bytes);
    void setPath(const std::string &newPath);
    void setState(FileReadState newState);

    ~FtFile();
};
