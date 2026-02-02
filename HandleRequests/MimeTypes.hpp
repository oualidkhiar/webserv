#pragma once
#include <map>
#include <string>
class MimeTypes
{
    private :
    static std::map <std::string , std::string> types;
    public :
    static void init();
    static std::string  getType(const std::string & extention);
};