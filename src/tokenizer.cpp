#include "../includes/tokenizer.hpp"

tokenizer::tokenizer(std::string filename): file(filename){}

tokenizer::~tokenizer() {
    for (int i = 0; i < tokens.size(); i++) {
        delete tokens[i];
    }
    file.close();
}

void tokenizer::createToren(type t, std::string data)
{
    Token *tok = new Token(t, data);
    this->tokens.push_back(tok);
}

void skipe_spaces(std::string& str, int& index) {
    while (str[index] == 32 || (str[index] >= 9 && str[index] <= 13)) {index++;}
}

std::string handleString(std::string& str, int& index)
{
	int     start;

    start = index;
	while 
	(
		index < str.length() && \
        str[index] != 32 && \
        (str[index] < 9 || str[index] > 13) && \
        str[index] != '{' && \
        str[index] != '}' && \
        str[index] != ';'
	)
	{index++;}
	return str.substr(start, index - start);
}

void tokenizer::extructToken(std::string& line, int& index)
{
    skipe_spaces(line, index);
    if (line[index] == '}') {
        this->createToren(CLOSEBRACKETS, "}");
        index++;
    }
    else if (line[index] == '{') {
        this->createToren(OPENBRACKETS, "{");
        index++;
    }
    else if (line[index] == ';') {
        this->createToren(SEMICOLON, ";");
        index++;
    }
    else if (line[index] == '#')
        index = line.length();
	else {
		std::string buffer = handleString(line, index);
		if (buffer.length() > 0)
			this->createToren(STRING, buffer);
	}
}

void tokenizer::tokenizerStart()
{
    std::string line;
    std::string subline;
    int         index;

    while (getline(this->file, line))
    {
        index = 0;
        while (index < line.length())
        {
            this->extructToken(line, index);
        }
    }
}

void tokenizer::printTokens()
{
    int index = 0;

    while (index < tokens.size())
    {
        if (((Token *)(tokens[index]))->t == STRING)
            std::cout << "STRING" << " " << ((Token *)(tokens[index]))->data<< std::endl;
        else if (((Token *)(tokens[index]))->t == SEMICOLON)
            std::cout << "SEMICOLON" << " " << ((Token *)(tokens[index]))->data<< std::endl;
        else if (((Token *)(tokens[index]))->t == OPENBRACKETS)
            std::cout << "OPENBRACKETS" << " " << ((Token *)(tokens[index]))->data << std::endl;
        else if (((Token *)(tokens[index]))->t == CLOSEBRACKETS)
            std::cout << "CLOSEBRACKETS" << " " << ((Token *)(tokens[index]))->data << std::endl;
        index++;
    }
}