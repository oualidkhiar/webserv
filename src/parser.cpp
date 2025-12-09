#include "../includes/parser.hpp"

parser::parser(tokenizer& tok): tokens(tok), error(false), index(0) {}

parser::~parser() {
    for (int i = 0; i < serversBlock.size(); i++) {
        clearAst(serversBlock[i]);
    }
}

AstNode *parser::creatNode(AstNode node) {
    AstNode *nd = new AstNode(node.type, node.name, node.args);
    if (!nd) {
        return NULL;
    }
    return nd;
}

void clearAst(AstNode *root) {
    if (!root)
        return ;
    for (int i = 0; i < root->children.size(); i++) {
        if (root->children[i]->type == LOCATION_NODE) {
            for (int j = 0; j < root->children[i]->children.size(); j++) {
                delete root->children[i]->children[j];
            }
            delete root->children[i];
        }
        else {
            delete root->children[i];
        }
    }
    delete root;
}

bool parser::expectedTokenType(type exp)
{
    if (index >= tokens.getTokensSize()) {
        std::cout << "unexpected EOF" << std::endl;
        this->error = true;
        return false;
    }
    Token *tok = peekToken();
    if (!tok) {
        this->error = true;
        return false;
    }
    if (exp != tok->t)
    {
        std::cout << "unexpected token" << " " << tok->data << std::endl;
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

AstNode *parser::parseData()
{
    AstNode node;
    Token *tok;

    tok = peekToken();
    if (!tok)
        return NULL;
    node.type = DATA_NODE;
    node.name = tok->data;
    advanceToken();
    tok = peekToken();
    while (index < tokens.getTokensSize() && tok && tok->t != SEMICOLON && tok->t != OPENBRACKETS && tok->t != CLOSEBRACKETS) {
        node.args.push_back(tok->data);
        advanceToken();
        tok = peekToken();
        if (!tok)
            return NULL;
    }
    if (!expectedTokenType(SEMICOLON))
        return NULL;
    return (creatNode(node));
}

AstNode *parser::parseLocationBlock()
{
    Token *tok = peekToken();

    if (!expectedTokenType(STRING))
        return NULL;
    if (!expectedTokenType(OPENBRACKETS))
        return NULL;
    AstNode *root = new AstNode(LOCATION_NODE, "location", {tok->data});
    while (peekToken() != NULL && peekToken()->t == STRING) {
        AstNode *child = parseData();
        if (child == NULL || this->error)
            break ;
        root->children.push_back(child);
    }
    if (this->error || !expectedTokenType(CLOSEBRACKETS)) {
        clearAst(root);
        return NULL;
    }
    return root;
}

AstNode *parser::parseServerBlock()
{
    if (!peekToken() || peekToken()->data != "server")
        return NULL;
    this->advanceToken();
    if (!expectedTokenType(OPENBRACKETS))
        return NULL;
    AstNode *root = new AstNode(SERVER_NODE, "server", {});
    while (peekToken() != NULL && peekToken()->t == STRING)
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
    std::cout << this->error << std::endl;
    if (this->error || root == NULL)
        return ;
    this->serversBlock.push_back(root);
    if (index < tokens.getTokensSize())
        this->startParser();
}


// --------------------------------------------------------------- 
void printData(AstNode *data) {
    std::cout << data->name << " : ";
    int k = 0;
    while (k < data->args.size()) {
        std::cout << data->args[k] << ", ";
        k++;
    }
    std::cout << std::endl;
}

void printLoation(AstNode * loctionchildren) {
    std::cout << "name " << loctionchildren->name << std::endl;
    for (int i = 0; i < loctionchildren->children.size(); i++) {
        printData(loctionchildren->children[i]);
    }
}

void parser::printParser() {
    for (int i = 0; i < serversBlock.size(); i++) {
        std::cout << "name " << serversBlock[i]->name << std::endl;
        std::vector<AstNode *> children = serversBlock[i]->children;
        for (int j = 0; j < children.size(); j++) {
            if (children[j]->type == LOCATION_NODE) {
                printLoation(children[j]);
            }
            else {
                printData(children[j]);
            }
        }
    }
}