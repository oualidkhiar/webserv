#ifndef CONFIG_HPP
#define CONFIG_HPP

#include "./tokenizer.hpp"

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
	std::string									fileName;
	void buildServersConfig();								// method to build the list of servers it called at the contruct time

public:

    config(std::string FileName);
    ~config();

	serverConfig *getSerevrConfig(int index);
	bool CheckParse();
	int ServersNumber();

};


#endif