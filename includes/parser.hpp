#ifndef PARSER_HPP
#define PARSER_HPP

#include "./tokenizer.hpp"

enum nodeType {
	SERVER_NODE,
    LOCATION_NODE,
    DATA_NODE
};

struct  AstNode {
    nodeType type;
    std::string name;
    std::vector<std::string> args;
    std::vector<AstNode *> children;
	AstNode() {}
    AstNode(nodeType type, std::string name, std::vector<std::string> args): type(type), name(name), args(args) {}
};

class parser {
private:

    tokenizer&  tokens;
    std::vector<AstNode *> serversBlock;
	int			index;
    bool        error;


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
    AstNode *peekNode(int index)
    {
        return serversBlock[index];
    }
    AstNode *advanceNode(int& index)
    {
        AstNode *ret = serversBlock[index];
        index++;
        return ret;
    }
};

void clearAst(AstNode *root);

#endif