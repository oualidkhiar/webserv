#include "StringManip.hpp"

std::string StringManip::get_token(std::string string, char delimiter)
{
    std ::string token;
    size_t pos = string.find(delimiter);
    if (pos != std::string::npos)
    {
        token = string.substr(0, pos);
        string = string.substr(pos + 1, string.length() - (pos + 1));
        while (string[0] == delimiter)
            string.erase(0, 1);
        return (token);
    }
    return ("");
}

std::pair<std::string , std::string> StringManip::split_two(std::string line, char delimiter)
{
    std::pair<std::string , std::string> pair;
    size_t npos = line.find(delimiter);
    pair.first = line.substr(0 , npos);
    pair.second = line.substr((npos + 1));
    return (pair);
}
