#include "../includes/config.hpp"

config::config(std::string FileName): fileName(FileName){
    this->error = false;
    this->buildServersConfig();
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

void config::buildServersConfig( void )
{
    tokenizer fileData(fileName);

    fileData.tokenizerStart();
    fileData.printTokens();
    // fileData.parseTokens();
    // this->fillServersData(fileData);
}

bool config::CheckParse() {
	return this->error;
}
int config::ServersNumber() {
	return servers.size();
}