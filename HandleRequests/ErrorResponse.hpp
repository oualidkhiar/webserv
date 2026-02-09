#include <cstddef>
#include <map>
#include <string>

class ErrorResponse
{
private:
    static std::map<int, std::string> initErrorPages();
    static std::string getDefaultErrorPage(int code);
    static std::string getStatusMessage(int code);

public:
    static std::pair<unsigned char *, size_t> getErrorResponse(int code);
};