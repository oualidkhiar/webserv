#include "global.hpp"
#include <utility>

std::map<int, std::string> defaultPages;

void initDefaultPages() {
    defaultPages.insert(std::make_pair(400, "/tmp/defaultErrorPages/400.html"));
    defaultPages.insert(std::make_pair(403, "/tmp/defaultErrorPages/404.html"));
    defaultPages.insert(std::make_pair(404, "/tmp/defaultErrorPages/400.html"));
    defaultPages.insert(std::make_pair(405, "/tmp/defaultErrorPages/400.html"));
    defaultPages.insert(std::make_pair(413, "/tmp/defaultErrorPages/400.html"));
    defaultPages.insert(std::make_pair(414, "/tmp/defaultErrorPages/400.html"));
    defaultPages.insert(std::make_pair(500, "/tmp/defaultErrorPages/400.html"));
    defaultPages.insert(std::make_pair(501, "/tmp/defaultErrorPages/400.html"));
    defaultPages.insert(std::make_pair(503, "/tmp/defaultErrorPages/400.html"));
    defaultPages.insert(std::make_pair(505, "/tmp/defaultErrorPages/400.html"));
    defaultPages.insert(std::make_pair(504, "/tmp/defaultErrorPages/400.html"));
}