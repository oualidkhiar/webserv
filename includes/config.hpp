#ifndef CONFIG_HPP
#define CONFIG_HPP

#include <iostream>
#include <string>
#include <vector>
#include <unordered_map>
#include <unordered_set>
#include <sys/types.h>
#include <unistd.h>
#include <fcntl.h>
#include <fstream>

struct location {

	std::unordered_set<std::string>	allowMethods;
	size_t							clientMaxSizeBody;

};

struct serverConfig {

    int                         					Port;
	std::vector<std::string>						ServerNames;
	std::string										rootPath;
	std::vector<std::string>						indexFiles;
    std::unordered_map<int, std::string>    		errorPage;
	std::unordered_map<std::string, location *>		Locations;

};

class config {
private:

	std::vector<serverConfig *>					servers;
	bool										error;
	std::ifstream								file;
	void buildServersConfig();								// method to build the list of servers it called at the contruct time
	std::vector<std::string>& readServerBlocFromFile();
	void parseServerBlock(std::vector<std::string>& serverBlock);

public:
    config(std::string FileName);
    ~config();
	serverConfig *getSerevrConfig(int index);
	bool CheckParse() {
		return this->error;
	}
	int ServersNumber() {
		return servers.size();
	}
};


#endif