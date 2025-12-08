#include "../includes/parser.hpp"

parser::parser(tokenizer& tok): tokens(tok), error(false), index(0) {}

parser::~parser() {
    for (int i = 0; i < this->serversBlock.size(); i++) {
        for (int j = 0; j < serversBlock[i]->children.size(); j++) {
            delete serversBlock[i]->children[j];
        }
        delete serversBlock[i];
    }
}

AstNode *parser::creatNode(AstNode node) {
    AstNode *nd = new AstNode(node.type, node.name, node.args);
    if (!nd) {
        return NULL;
    }
    return nd;
}

void parser::parseToken(int index) {

}

bool parser::expectedTokenData(std::string exp) {
    std::string token = this->tokens.getToken(this->index)->data;
    if (exp != token)
    {
        std::cout << "unexpected token" << " " << token << std::endl;
        this->error = true;
        return false;
    }
    this->advanceToken();
    return true;
}

bool parser::expectedTokenType(type exp) {
    type tokenType = this->tokens.getToken(this->index)->t;
    std::string token = this->tokens.getToken(this->index)->data;
    if (exp != tokenType)
    {
        std::cout << "unexpected token" << " " << token << std::endl;
        this->error = true;
        return false;
    }
    this->advanceToken();
    return true;
}

Token *parser::peekToken() {
    return this->tokens.getToken(index);
}

void parser::advanceToken() {
    this->index++;
}

void clearAst(AstNode *root) {
    for (int i = 0; i < root->children.size(); i++) {
        delete root->children[i];
    }
    delete root;
}

AstNode *parser::parseData()
{
    AstNode node;

    node.type = DATA_NODE;
    node.name = peekToken()->data;
    advanceToken();
    while (index < tokens.getTokensSize() && peekToken()->t != SEMICOLON && peekToken()->t != OPENBRACKETS && peekToken()->t != CLOSEBRACKETS) {
        node.args.push_back(peekToken()->data);
        advanceToken();
    }
    if (!expectedTokenType(SEMICOLON))
        return NULL;
    return (creatNode(node));
}

AstNode *parser::parseLocationBlock() {
    Token *tok = peekToken();

    if (!expectedTokenType(STRING))
        return NULL;
    if (!expectedTokenType(OPENBRACKETS))
        return NULL;
    AstNode *root = new AstNode(LOCATION_NODE, "location", {tok->data});
    while (peekToken()->t == STRING) {
        AstNode *child = parseData();
        if (child == NULL || this->error)
            break ;
    }
    if (this->error || !expectedTokenType(CLOSEBRACKETS)) {
        clearAst(root);
        return NULL;
    }
    return root;
}


AstNode *parser::parseServerBlock()
{
    if (!expectedTokenData("server"))
        return NULL;
    if (!expectedTokenType(OPENBRACKETS))
        return NULL;
    AstNode *root = new AstNode(SERVER_NODE, "server", {});
    while (peekToken()->t == STRING)
    {
        AstNode *child;
        if (peekToken()->data == "location") {
            this->advanceToken();
            child = parseLocationBlock();
        }
        else {
            child = parseData();
        }
        if (this->error || child == NULL)
            break ;
        root->children.push_back(child);
    }
    if (this->error || !expectedTokenType(CLOSEBRACKETS)) {
        clearAst(root);
        return NULL;
    }
    return root;
}

void parser::startParser()
{
    AstNode *root = parseServerBlock();
    if (this->error || root == NULL)
        return ;
    this->serversBlock.push_back(root);
    if (index < tokens.getTokensSize())
        this->startParser();
}


void parser::printParser() {
    for (int i = 0; i < serversBlock.size(); i++) {

    }
}