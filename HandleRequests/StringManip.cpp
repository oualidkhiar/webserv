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
        token = string.substr(0, pos);
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

int hex_to_num(const std::string hex)
{
    int result = 0;
    for (size_t i = 0; i < hex.size(); ++i)
    {
        result *= 16;

        if (std::isdigit(hex[i]))
            result += hex[i] - '0';
        else if (hex[i] >= 'A' && hex[i] <= 'F')
            result += hex[i] - 'A' + 10;
        else if (hex[i] >= 'a' && hex[i] <= 'f')
            result += hex[i] - 'a' + 10;
        else
            return -1;
    }
    return result;
}

int stringToNumber(std::string s)
{
    int number;
    number = 0;

    int i = 0;
    while (s[i])
    {
        if (std::isdigit(s[i]) == false)
            return (-1);
        number = (number * 10) + (s[i] - '0');
        i++;
    }
    return (number);
}

void exit_error(const char *s)
{
    std::cout << s << std::endl;
    exit(1);
}

void write_str(const char *des, const char *s)
{
    std::cout << "----------------------------------" << des << "-------------------------------------" << std::endl;
    std::cout << "----------------------------------------------------------------------------" << std::endl;
    std::cout << s << std::endl;
    std::cout << "----------------------------------------------------------------------------" << std::endl;
}