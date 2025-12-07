#include "../includes/tokenizer.hpp"

parser::parser(std::string filename): tokenizer(filename), error(false) {
    this->tokenizerStart();
}

parser::~parser() {}

void parser::startParser()
{
    for (int i = 0; i < this->tokens.size(); i++) {
        // todo::
    }
}