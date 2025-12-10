
#include "./includes/tokenizer.hpp"
#include "./includes/parser.hpp"
#include <map>
using namespace std;
int main() {
    
    tokenizer tok("./config/test.config");
    tok.tokenizerStart();
    // tok.printTokens();
    parser p(tok);
    p.startParser();
    // p.printParser();
}

// class Solution {
// public:
//     string countAndSay(int n) {
        
//     }
// };

// int main () {
//     Solution sol;
//     vector<string> ve = {"flower","flower","flower","flower"};

//     cout << sol.longestCommonPrefix(ve) << endl;
// }