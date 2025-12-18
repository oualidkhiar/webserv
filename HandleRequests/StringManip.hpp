#pragma oncc
#include <string>
class StringManip
{
public:
    static std::string get_token(std::string string, char delimiter);
    static std::pair<std::string , std::string> split_two (std::string string , char delimiter);
};