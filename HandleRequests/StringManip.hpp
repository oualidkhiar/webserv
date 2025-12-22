
#pragma once
#include <string>
class StringManip
{
public:
    static std::string get_token(std::string string, char delimiter);
    static std::pair<std::string , std::string> split_two (std::string string , char delimiter);
    static std::string strtrim(const std :: string string);

};

int hex_to_num(std::string hex);
int stringToNumber(std::string s);
void exit_error(const char * s);
void write_str(const char * des , const char *s);