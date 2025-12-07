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

enum nodeType {
    BLOCK_NODE,
    DIRECTIVE_NODE
};


struct Token {
    type t;
    std::string data;
    Token(type t, std::string d): t(t), data(d) {}
};

struct  AstNode {
    nodeType type;
    std::string name;
    std::vector<std::string> args;
    std::vector<AstNode *> children;
    AstNode(nodeType type, std::string name, std::vector<std::string> args): type(type), name(name), args(args) {}
};


class tokenizer {
protected:

    std::ifstream           file;
    std::vector<Token *>    tokens;

    void createToren(type t, std::string data);
    void extructToken(std::string& line, int& index);

public:

    tokenizer(std::string fileName);
    ~tokenizer();
    void tokenizerStart();
    void printTokens();

};



class parser: public tokenizer {
private:

    bool     error;
    AstNode *root;

public:

    parser(std::string filename);
    ~parser();

    void startParser();

};

void skipe_spaces(std::string& str, int& index);
std::string handleString(std::string& str, int& index);

#endif
