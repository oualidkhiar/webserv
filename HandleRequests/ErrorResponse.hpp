#include <cstddef>
#include <map>
#include <string>

class ErrorResponse
{
private:
    static std::map<int, std::string> initErrorPages();

public:
    static std::pair<unsigned char *, size_t> getErrorResponse(int code);
};