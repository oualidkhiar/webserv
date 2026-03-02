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

// this is modified to not trim the first string
std::pair<std::string, std::string> StringManip::split_two(std::string line, char delimiter)
{
    std::pair<std::string, std::string> pair;
    size_t pos = line.find(delimiter);

    if (pos != std::string::npos)
    {
        // pair.first = StringManip::strtrim(line.substr(0, pos));
        pair.first = line.substr(0, pos); // no trim.
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

std::string StringManip::toLowerCase(const std::string s)
{
    std::string result = s;
    for (size_t i = 0; i < result.length(); i++)
        result[i] = std::tolower(result[i]);
    return result;
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
    int number = 0;
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

std::vector<std::string> ft_split(std::string string, std::string delimiter)
{
    std::vector<std::string> words;
    while (true)
    {
        size_t pos = string.find(delimiter);
        if (pos == 0)
            string.erase(0, delimiter.length());
        else if (pos == std::string::npos)
        {
            words.push_back(string);
            break;
        }
        else
        {
            words.push_back(string.substr(0, pos));
            string.erase(0, pos + delimiter.length());
        }
    }
    return (words);
}

int tokensSize(std::string string, std::string delimiter)
{
    int size = 0;
    if (string.empty() == true)
        return (0);
    while (true)
    {
        size_t pos = string.find(delimiter);
        if (pos == std::string::npos)
            return (size + 1);
        else if (pos == 0)
            string.erase(0, delimiter.length());
        else
        {
            size++;
            string.erase(0, pos + 1);
        }
    }
    return (size);
}

void printVector(std::vector<unsigned char> vec)
{
    std::cout<<"INside PRint VECTIOR"<<std::endl;
    size_t size = vec.size();
    size_t i = 0;
    while (i < size)
        std::cout << vec[i++];
    std::cout << std::endl;
}

bool StringManip::isAllUppercase(const std::string &string)
{
	for (size_t i = 0; i < string.size(); i++)
		if (std::islower(string[i]))
			return false;
	return true;
}