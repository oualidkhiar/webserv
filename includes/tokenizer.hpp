#ifndef TOKENIZER_HPP
#define TOKENIZER_HPP

#include <iostream>
#include <string>
#include <vector>
#include <unordered_map>
#include <unordered_set>
#include <sys/types.h>
#include <unistd.h>
#include <fcntl.h>
#include <fstream>


enum type {
    STRING,
    SEMICOLON,
    OPENBRACKETS,
    CLOSEBRACKETS
    // EDONTIFIER
};

struct Token {
    type t;
    std::string data;
    Token(type t, std::string d): t(t), data(d) {}
};

class tokenizer {
private:

    std::ifstream           file;
    std::vector<Token *>    tokens;

    void createToren(type t, std::string data);
    void extructToken(std::string& line, int& index);

public:

    tokenizer(std::string fileName);
    ~tokenizer();
    void tokenizerStart();
    void parseTokens();
    void printTokens();

};

void skipe_spaces(std::string& str, int& index);
std::string handleString(std::string& str, int& index);

#endif