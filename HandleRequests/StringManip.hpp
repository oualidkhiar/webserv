
#pragma once
#include <string>
#include <vector>
class StringManip
{
public:
    static std::string get_token(std::string string, char delimiter);
    static std::pair<std::string , std::string> split_two (std::string string , char delimiter);
    static std::string strtrim(const std :: string string);
    static std::string toLowerCase(const std::string string);
    static bool isAllUppercase(const std::string &string);
};


int hex_to_num(std::string hex);
int stringToNumber(std::string s);
void exit_error(const char * s);
void write_str(const char * des , const char *s);
std::vector<std::string> ft_split(std::string string , std::string delimiter);
int tokensSize(std::string string , std::string delimiter);
void printVector(std::vector<unsigned char> vec);