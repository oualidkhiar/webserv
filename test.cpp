
#include "./includes/tokenizer.hpp"
#include "./includes/parser.hpp"
#include "./includes/config.hpp"
#include <map>
using namespace std;


#include <ctime>
#include <iostream>

int main() {
    time_t now = time(nullptr);
    std::cout << now - now << std::endl;
    sleep(10);
    time_t leter = time(nullptr);
    cout << "after ten seond: " << leter - now << endl;
}