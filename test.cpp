
#include "./includes/tokenizer.hpp"
#include "./includes/parser.hpp"
#include "./includes/config.hpp"
#include <map>
using namespace std;
int main() {

    // // config conf("./config/test.config");
    // // conf.buildServersConfig();
    // // if (!conf.CheckParse())
    // //     conf.printServer();

    // int fd = open("README.md", O_RDONLY);

    // char buffer[500];
    // int bread = read(fd, buffer, 500);

    // // std::cout << bread << std::endl;

    // for (int i = 0; i < bread; i++) {
    //     std::cout << buffer[i];
    // }
    // std::cout << std::endl;

    const char *filename = "cc.c";
    std::remove(filename);
}
