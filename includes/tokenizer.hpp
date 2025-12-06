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
    CLOSEBRACKETS,
    EDONTIFIER
};

struct Token {
    type t;
    std::string data;
};

class tokenizer {
private:
    FILE *file;
    std::vector<Token *> tokens;
public:
    tokenizer(FILE *file);
    ~tokenizer();
};

tokenizer::tokenizer(FILE *file): file(file){}

tokenizer::~tokenizer() {}


#endif