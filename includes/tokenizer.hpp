#ifndef TOKENIZER_HPP
#define TOKENIZER_HPP

#include <iostream>
#include <cstdlib>
#include <string>
#include <vector>
#include <map>
#include <set>
#include <sys/types.h>
#include <unistd.h>
#include <fcntl.h>
#include <fstream>


enum type {
    STRING,
    SEMICOLON,
    OPENBRACKETS,
    CLOSEBRACKETS
};


struct Token {
    type t;
    std::string data;
    Token(type t, std::string d): t(t), data(d) {}
};


class tokenizer {
protected:

    std::ifstream           file;
    bool                    error;
    std::vector<Token *>    tokens;

    void createToren(type t, std::string data);
    void extructToken(std::string& line, size_t& index);

public:

    tokenizer(std::string fileName);
    ~tokenizer();

    Token *getToken(size_t index);

    int getTokensSize();

    void tokenizerStart();
    void printTokens();
    bool checkTokenizeError();

};

void skipe_spaces(std::string& str, size_t& index);
std::string handleString(std::string& str, size_t& index);

#endif
