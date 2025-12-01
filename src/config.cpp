#include "../includes/config.hpp"

config::config(std::string FileName): file(FileName) {
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

void strtrim(std::string& str) {
	int start = 0;
	int end = str.length() - 1;

	while (str[start] == 32 || (str[start] >= 9 && str[start] <= 13)) {start++;}
	while (str[end] == 32 || (str[end] >= 9 && str[end] <= 13)) {end--;}
	str.substr(start, end - start);
}

std::vector<std::string>& config::readServerBlocFromFile()
{
    std::vector<std::string>    ret;
    std::string                 line;
    bool                        firstLine;

    firstLine = false;
    while (true)
    {
        getline(this->file, line);
		strtrim(line);
        if (!firstLine) {
            if (line.find("server") != std::string::npos) {
                firstLine = true;
				if (line.find("{") == std::string::npos)
				ret.push_back(line);
			}
            else {
                for (char c : line) {
                    if (c != '\n' && c != 32 && c != 9) {
						this->error = true;
						return ret;
					}
                }
				continue ;
            }
        }
    }

    return (ret);
}

// void config::parseServerBlock(std::vector<std::string>& serverBlock)
// {
// 	serverConfig *server;

//     server = new (serverConfig);
// 	if (ports.count(server->Port))
//     {
//         std::cout << "Error : duplicate the port not suported" << std::endl;
//         delete server;
//         this->error = true;
//         return ;
//     }
// }

void config::buildServersConfig( void )
{
    std::vector<std::string>             serverBlock;
    std::unordered_set<int>              ports;

    while (true)
    {
        serverBlock = readServerBlocFromFile();
        if (serverBlock.size() == 0) {
            break ;
        }
        parseServerBlock(serverBlock);
        if (this->error == true) {
            return ;
        }
        // ports.insert(server->Port);
        // this->servers.push_back(server);
        // continue build the other servers config if it exist
    }
    if (this->servers.size() == 0) {
        this->error = true;
    }
}
