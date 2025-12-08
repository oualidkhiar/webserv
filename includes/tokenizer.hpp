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
};


struct Token {
    type t;
    std::string data;
    Token(type t, std::string d): t(t), data(d) {}
};


class tokenizer {
protected:

    std::ifstream           file;
    std::vector<Token *>    tokens;

    void createToren(type t, std::string data);
    void extructToken(std::string& line, int& index);

public:

    tokenizer(std::string fileName);

    Token *getToken(int index) {
        if (index < tokens.size())
            return tokens[index];
        return NULL;
    }

    int getTokensSize() {
        return tokens.size();
    }

    ~tokenizer();
    void tokenizerStart();
    void printTokens();

};

void skipe_spaces(std::string& str, int& index);
std::string handleString(std::string& str, int& index);

#endif
