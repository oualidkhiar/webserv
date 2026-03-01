#pragma once
#include <string>
#include <vector>
#include <cstdio>

enum FileReadState
{
    NOT_OPENED,
    FILE_READING,
    FILE_WRITING,
    FILE_FINISHED,
};

class FtFile
{
private:
    int fd;
    size_t sented_bytes;
    std::string path;
    FileReadState state;
    size_t file_size;
    bool remove_file;

public:
    FtFile(const std::string &path);
    void setRemoveFile(bool t_f);
    bool shouldRemovingFile();
    // Getters
    size_t getFileSize();
    int getFd() const;
    size_t getSentedBytes() const;
    const std::string &getPath() const;
    FileReadState getState() const;
    void incrementSentedBytes(size_t amount);
    std::vector<unsigned char> readFile();
    void FtFile::writeToFile(const std::vector<unsigned char> &data, bool close);
    // Setters
    void setFileSize(size_t size);
    void setFd(int newFd);
    void setSentedBytes(size_t bytes);
    void setPath(const std::string &newPath);
    void setState(FileReadState newState);
    void ft_close();

    ~FtFile();
};
