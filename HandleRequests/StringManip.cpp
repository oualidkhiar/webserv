#include "StringManip.hpp"
#include <algorithm>
#include <iostream>
#include <cmath>

std::string StringManip::get_token(std::string string, char delimiter)
{
    std ::string token;
    size_t pos = string.find(delimiter);
    if (pos != std::string::npos)
    {
        std::cout << "strtrim returned " << std::endl;
        std::cout << "pos = " << pos << std::endl;
        token = string.substr(0, pos);
        std::cout << "token = " << token << std::endl;
        return (token);
    }
    return (StringManip::strtrim(string));
}

std::pair<std::string, std::string> StringManip::split_two(std::string line, char delimiter)
{
    std::pair<std::string, std::string> pair;
    size_t pos = line.find(delimiter);

    if (pos != std::string::npos)
    {
        pair.first = StringManip::strtrim(line.substr(0, pos));
        pair.second = StringManip::strtrim(line.substr(pos + 1));
    }
    return pair;
}

std::string StringManip::strtrim(const std ::string s)
{
    size_t start = 0, end = s.length();

    while (start < end && std::isspace((s[start])))
        start++;
    if (start == end)
        return ("");
    while (end > start && std::isspace((s[end - 1])))
        end--;
    return s.substr(start, end - start);
}

int hex_to_num(std::string hex)
{
    int results = 0;
    for (int i = 0; i < hex.length(); i++)
    {
        if (std::isdigit(hex[i]))
        {
            results += (hex[i] - '0') * std::pow(16, i);
        }
        else if ((hex[i] >= 'A' && hex[i] <= 'F'))
            results += (hex[i] - 'A' + 1) * std::pow(16, i);
        else
            return (-1);
    }
}

int stringToNumber(std::string s)
{
    int number;
    
    int i = 0;
    while(s[i] && std::isdigit(s[i]))
    {

    }
}