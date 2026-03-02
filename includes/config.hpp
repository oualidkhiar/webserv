#ifndef CONFIG_HPP
#define CONFIG_HPP

#include "./parser.hpp"

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
	std::pair<int, std::string>						redirection;

	location(): clientMaxSizeBody(0), autoindex(false) {}

};

struct serverConfig {
private:

    std::vector<int>								Port;
	size_t											clientMaxSizeBody;
	std::vector<std::string>						ServerNames;
	std::string										rootPath;
	std::vector<std::string>						indexFiles;
	bool											autoindex;
	std::set<std::string>							allowMethods;
	std::pair<int, std::string>						redirection; 

public:
	std::map<std::string, location *>				Locations;
	serverConfig(): clientMaxSizeBody(0), autoindex(false) {}
	// geters for Global data
	std::vector<int>& getPorts();
	size_t getMaxBodySize();
	std::vector<std::string>& getServerNames();
	std::string&	getRootPath();
	std::vector<std::string>& getIndexFiles();
	bool getAutoIndexFlag();
	std::set<std::string>& getAllowedMethods();
	std::pair<int, std::string>& getRedirection();
	// seters for Global data
	void setPort(int port);
	void setMaxBodySize(size_t clientBody);
	void setServerName(std::string serverName);
	void setRootPath(std::string path);
	void setIndexFile(std::string indexFile);
	void setAutoIndexFlag();
	void setAllowedMethod(std::string method);
	void setRedirection(int code, std::string url);

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