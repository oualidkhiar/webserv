#ifndef CONFIG_HPP
#define CONFIG_HPP

#include "./parser.hpp"
#define DEFAULT_SIZE 1000000

struct location {

	std::string						key;
	std::unordered_set<std::string>	allowMethods;
	size_t							clientMaxSizeBody;

};

struct serverConfig {

    std::vector<int>								Port;
	size_t											clientMaxSizeBody;
	std::vector<std::string>						ServerNames;
	std::string										rootPath;
	std::vector<std::string>						indexFiles;
    std::unordered_map<int, std::string>    		errorPage;
	std::unordered_map<std::string, location *>		Locations;
	serverConfig(): clientMaxSizeBody(-1) {}

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
	void customSever(std::vector<AstNode *> root, serverConfig *server);
	void customDataServer(AstNode *node, serverConfig *server);
	void customDataLocation(AstNode *node, location *loc);
	void customLocation(AstNode *node, location *server);
	void printServer();
};

void clearServer(serverConfig *server);
bool validateDataBlock(serverConfig *server); 

#endif