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
	// std::pair<int, std::string>						redirect_return; 

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
	// std::pair<int, std::string>						redirect_return; 

public:
	std::map<std::string, location *>				Locations;
	serverConfig(): clientMaxSizeBody(0), autoindex(false) {}
	// geters for Global data
	std::vector<int>& getPorts() {return this->Port;}
	size_t getMaxBodySize() {return clientMaxSizeBody;}
	std::vector<std::string>& getServerNames() {return this->ServerNames;}
	std::string&	getRootPath() {return this->rootPath;}
	std::vector<std::string>& getIndexFiles() {return this->indexFiles;}
	bool getAutoIndexFlag() {return this->autoindex;}
	std::set<std::string>& getAllowedMethods() {return this->allowMethods;}
	// seters for Global data
	void setPort(int port) {this->Port.push_back(port);}
	void setMaxBodySize(size_t clientBody) {this->clientMaxSizeBody = clientBody;}
	void setServerName(std::string serverName) {this->ServerNames.push_back(serverName);}
	void setRootPath(std::string path) {this->rootPath = path;}
	void setIndexFile(std::string indexFile) {this->indexFiles.push_back(indexFile);}
	void setAutoIndexFlag() {this->autoindex = true;}
	void setAllowedMethod(std::string method) {this->allowMethods.insert(method);}

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