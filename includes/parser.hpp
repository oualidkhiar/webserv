#ifndef PARSER_HPP
#define PARSER_HPP

#include "./tokenizer.hpp"

enum nodeType {
	SERVER_NODE,
    LOCATION_NODE,
    DATA_NODE
};

struct  AstNode {
    nodeType                    type;
    std::string                 name;
    std::vector<std::string>    args;
    std::vector<AstNode *>      children;
	AstNode() {}
    AstNode(nodeType type, std::string name, std::vector<std::string> args){
        this->type = type;
        this->name = name;
        this->args = args;
    }
};

class parser {
private:

    tokenizer&  tokens;
    std::vector<AstNode *> serversBlock;
    bool        error;
	int			index;


	bool expectedTokenType(type t);
    AstNode *creatNode(AstNode node);
	AstNode *parseData();
	AstNode *parseLocationBlock();
	AstNode *parseServerBlock();
	void advanceToken();
	Token *peekToken();

public:

    parser(tokenizer& tok);
    ~parser();

    void startParser();
    bool checkErrorParse();

    void printParser();
    AstNode *peekNode(size_t index);
};

void clearAst(AstNode *root);

#endif