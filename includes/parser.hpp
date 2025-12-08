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

	bool expectedTokenData(std::string exp);
	bool expectedTokenType(type t);

public:

    parser(tokenizer& tok);
    ~parser();

    void startParser();
    void parseToken(int tokIndex);
    AstNode *creatNode(AstNode node);

	AstNode *parseData();
	AstNode *parseLocationBlock();
	AstNode *parseServerBlock();
	void advanceToken();
	Token *peekToken();

    void printParser();

};


#endif