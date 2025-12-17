#pragma oncc
#include <string>

class StringManip
{
    public : 
    std::string  get_token(std::string string , std::string delimiter)
    {
        std ::string token;
        size_t pos = string.find(delimiter);
        if (pos != std::string::npos)
        {
            token = string.substr(0 , pos);
            

        }

    }
};