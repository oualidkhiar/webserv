#ifndef CONFIG_HPP
#define CONFIG_HPP

#include "./parser.hpp"
#define DEFAULT_SIZE 1000000

struct location {

	std::string										key;
	std::set<std::string>							allowMethods;
	size_t											clientMaxSizeBody;
	std::string										rootPath;
	std::vector<std::string>						indexFiles;
	bool											autoindex;
	std::vector<std::string>						cgi_extension;
	std::string										cgi_pass;
	std::string										upload_store;
	// std::pair<int, std::string>						redirect_return; 

	location(): clientMaxSizeBody(0), autoindex(false) {}

};

struct serverConfig {

    std::vector<int>								Port;
	size_t											clientMaxSizeBody;
	std::vector<std::string>						ServerNames;
	std::string										rootPath;
	std::vector<std::string>						indexFiles;
    std::map<int, std::string>    					errorPage;
	std::map<std::string, location *>				Locations;
	bool											autoindex;

	serverConfig(): clientMaxSizeBody(0), autoindex(false) {}

};

class config {
private:

	std::string									filename;
	bool										error;
	std::vector<serverConfig *>					servers;
	
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