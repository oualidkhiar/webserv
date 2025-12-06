
#include <iostream>
#include <string>
#include <vector>
#include <unordered_map>
#include <unordered_set>
#include <sys/types.h>
#include <unistd.h>
#include <fcntl.h>
#include <fstream>

int main() {

    int i = 0;

    while (i < 10) 
    {
        std::vector<std::string> s;
        s.push_back("walid");
        std::cout << s.size() << std::endl;
        i++;
    }
    return 0;
}