#include <cstddef>
#include <map>
#include <string>
#include "config.hpp"

class ErrorResponse
{
private:
    // static std::map<int, std::string> getErrorPagePath(serverConfig *serverConf);

public:
    static std::pair<unsigned char *, size_t> getErrorResponse(serverConfig *serverConf, int code);
};