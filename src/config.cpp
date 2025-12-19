#include "../includes/config.hpp"

config::config(std::string FileName): filename(FileName), index(0) {
    this->error = false;
}

config::~config()
{
    for (int i = 0; i < servers.size(); i++) {
        delete this->servers[i];
    }
}

serverConfig *config::getSerevrConfig( int index )
{
	return this->servers[index];
}

// void customSever(AstNode *server)
// {
//     if (server->type == DATA_NODE)
        
// }

void config::startEvaluation(parser& p)
{
    while (p.peekNode(index)->type == SERVER_NODE)
    {
        // customSever(p.advanceNode(index));
        if (this->error)
            break ;
    }
}

void config::buildServersConfig( void )
{
    tokenizer tok(filename);

    if (tok.checkTokenizeError()) {
        this->error = true;
        return ;
    }
    tok.tokenizerStart();
    tok.printTokens();
    if (tok.getTokensSize() == 0) {
        std::cout << "Error: empty or invalid configuration file" << std::endl;
        this->error = true;
        return ;
    }

    parser p(tok);

    p.startParser();
    p.printParser();
    if (p.checkErrorParse()) {
        this->error = true;
        return ;
    }
    startEvaluation(p);
}

bool config::CheckParse() {
	return this->error;
}
int config::ServersNumber() {
	return servers.size();
}