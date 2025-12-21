
#include "./includes/tokenizer.hpp"
#include "./includes/parser.hpp"
#include "./includes/config.hpp"
#include <map>
using namespace std;
int main() {

    config conf("./config/test.config");
    conf.buildServersConfig();
    if (!conf.CheckParse())
        conf.printServer();

}
