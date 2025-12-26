#ifndef CONFIG_HPP
#define CONFIG_HPP
#include <set>
#include <map>

#include "./parser.hpp"

struct location {

	std::set<std::string>	allowMethods;
	std::string rootPath;
	size_t							clientMaxSizeBody;


};

struct serverConfig {

    int                         					Port;
	size_t											clientMaxSizeBody;
	std::vector<std::string>						ServerNames;
	std::string										rootPath;
	std::vector<std::string>						indexFiles;
    std::map<int, std::string>    		errorPage;
	std::map<std::string, location *>		Locations;

};

class config {
private:

	// tokenizer									*tok;
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