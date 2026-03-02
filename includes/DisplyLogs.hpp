#pragma once
#include <iostream>
#include <ctime>
#include <iomanip>
#include <sstream>
#include "constent.hpp"

class DisplyLogs {
public:
    static void printCurrentAtion(const std::string& type, const std::string& message, const char *color) {
        std::cout << CYAN << "[" << getCurrentTime() << "] " << RESET
        << color << type << RESET << " " << message << std::endl;
    }

    static std::string getCurrentTime()
    {
        std::time_t now = std::time(NULL);
        std::tm *lt = std::localtime(&now);
        std::ostringstream oss;
        oss << std::setfill('0')
            << std::setw(2) << lt->tm_hour << ":"
            << std::setw(2) << lt->tm_min  << ":"
            << std::setw(2) << lt->tm_sec;
        return oss.str();
    }
    
};


// [12:41:03] [INFO ] [SERVER ] Listening on port 8080
// [12:41:05] [CONN ] [ACCEPT ] Client 127.0.0.1:53214 connected
// [12:41:05] [REQ  ] [GET    ] /index.html
// [12:41:05] [RESP ] [200    ] 532 bytes
// [12:41:06] [ERROR] [CGI    ] script timeout