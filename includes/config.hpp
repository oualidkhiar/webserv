#ifndef CONFIG_HPP
#define CONFIG_HPP

#include "./parser.hpp"

struct location {

	std::unordered_set<std::string>	allowMethods;
	size_t							clientMaxSizeBody;

};

struct serverConfig {

    int                         					Port;
	size_t											clientMaxSizeBody;
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
	std::string									filename;
	int											index;
	
public:
	
    config(std::string FileName);
    ~config();
	
	serverConfig *getSerevrConfig(int index);
	bool CheckParse();
	int ServersNumber();
	
	void buildServersConfig();
	void startEvaluation(parser& p);
};


#endif