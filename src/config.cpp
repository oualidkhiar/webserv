#include "../includes/config.hpp"

config::config(std::string FileName): filename(FileName), error(false) {}

config::~config()
{
    for (size_t i = 0; i < servers.size(); i++) {
		for (std::map<std::string, location *>::iterator it = servers[i]->Locations.begin(); it != servers[i]->Locations.end(); it++) {
			delete it->second;
		}
		delete servers[i];
    }
}

serverConfig *config::getSerevrConfig( int index )
{
	return this->servers[index];
}

bool config::CheckParse() {
	return this->error;
}

int config::ServersNumber() {
	return servers.size();
}

void config::customDataLocation(AstNode *node, location *loc)
{
	if (node->name == "root") {
		if (node->args.size() != 1) {
			std::cout << "Error : multiple roots declared at a line" << std::endl;
			this->error = true;
			return ;
		}
		else {
			loc->rootPath = node->args[0];
		}
	}
	else if (node->name == "client_max_body_size" && node->args.size() == 1) {
		for (size_t j = 0; j < node->args[0].length(); j++) {
			if (!isdigit(node->args[0][j])) {
				std::cout << "Error : invalide client max size body number detected" << "{" << node->args[0] << "}" << std::endl;
				this->error = true;
				return ;
			}
		}
		loc->clientMaxSizeBody = std::atoi((node->args[0].c_str()));
	}
	else if (node->name == "index") {
		for (size_t i = 0; i < node->args.size(); i++) {
			loc->indexFiles.push_back(node->args[i]);
		}
	}
	else if (node->name == "autoindex") {
		if (node->args[0] == "on") {
			loc->autoindex = true;
		}
		else if (node->args[0] != "off") { 
			std::cout << "Warning: Unknown word " << node->args[0] << " auto index will treat as default (off)" << std::endl; 
		}
	}
	else if (node->name == "methods") {
		for (size_t i = 0; i < node->args.size(); i++) {
			if (node->args[i] == "GET" || node->args[i] == "POST" || node->args[i] == "DELETE") {
				loc->allowMethods.insert(node->args[i]);
			}
			else {
				std::cout << "Error : Unknown method " << node->args[i]  << std::endl;
				this->error = true;
				return ;
			}
		}
	}
	else if (node->name == "cgi_extension") {
		for (size_t i = 0; i < node->args.size(); i++) {
			loc->cgi_extension.push_back(node->args[i]);
		}
	}
	else if (node->name == "cgi_pass") {
		if (node->args.size() != 1) {
			std::cout << "Warning : multiple values of cgi_pass; the first will be used" << std::endl;
		}
		loc->cgi_pass = node->args[0];
	}
	else {
		std::cout << "Warning: Unknown directive '" << node->name << "' " << "it will ignored" << std::endl;
	}
}

void config::customLocation(AstNode *LocationRoot, location *loc)
{
	loc->key = LocationRoot->args[0];
	std::vector<AstNode *> children = LocationRoot->children;
	for (size_t i = 0; i < children.size(); i++) {
		customDataLocation(children[i], loc);
		if (this->error) {
			break ;
		}
	}
}

void config::customDataServer(AstNode *node, serverConfig *server)
{
	if (node->name == "listen") {
		for (size_t i = 0; i < node->args.size(); i++) {
			if (node->args[i].length() > 5) {
				std::cout << "Error : Invalide port number " << node->args[i] << std::endl;
				this->error = true;
				return ;
			}
			for (size_t j = 0; j < node->args[i].length(); j++) {
				if (!isdigit(node->args[i][j])) {
					std::cout << "Error : invalide port number detected" << std::endl;
					this->error = true;
					return ;
				}
			}
			int port = std::atoi((node->args[i].c_str()));
			if (port <= 0 || port > 65535) {
				std::cout << "Error the port " << port << " outside the range 1 - 65535" << std::endl;
				this->error = true;
				return ;
			}
			server->setPort(std::atoi((node->args[i].c_str())));
		}
	}
	else if (node->name == "client_max_body_size" && node->args.size() == 1) {
		for (size_t j = 0; j < node->args[0].length(); j++) {
			if (!isdigit(node->args[0][j])) {
				std::cout << "Error : invalide client max size body number detected" << std::endl;
				this->error = true;
				return ;
			}
		}
		server->setMaxBodySize(std::atoi((node->args[0].c_str())));
	}
	else if (node->name == "server_name") {
		for (size_t i = 0; i < node->args.size(); i++) {
			server->setServerName(node->args[i]);
		}
	}
	else if (node->name == "root") {
		if (node->args.size() == 1) {
			server->setRootPath(node->args[0]);
		}
		else if (node->args.size() > 1) {
			std::cout << "Error : multiple roots declared invalid" << std::endl;
		}
	}
	else if (node->name == "index") {
		for (size_t i = 0; i < node->args.size(); i++) {
			server->setIndexFile(node->args[i]);
		}
	}
	else if (node->name == "autoindex") {
		if (node->args[0] == "on")
			server->setAutoIndexFlag();
	}
	else if (node->name == "methods") {
		for (size_t i = 0; i < node->args.size(); i++) {
			if (node->args[i] == "GET" || node->args[i] == "POST" || node->args[i] == "DELETE") {
				server->setAllowedMethod(node->args[i]);
			}
			else {
				std::cout << "Error : Unknown method " << node->args[i]  << std::endl;
				this->error = true;
				return ;
			}
		}
	}
	else {
		std::cout << "Warning: Unknown directive '" << node->name << "' " << "it will ignored in server block" << std::endl;
	}
}

void config::customSever(std::vector<AstNode *> root, serverConfig *server)
{
    for (size_t i = 0; i < root.size(); i++) {
        if (root[i]->type == LOCATION_NODE) {
            location *loc = new location();
            customLocation(root[i], loc);
            if (this->error) {
                delete loc;
                break ;
            }
            server->Locations.insert(std::make_pair(loc->key, loc));
        }
        else {
            customDataServer(root[i], server);
            if (this->error) {
                break ;
            }
        }
    }
}

void clearServer(serverConfig *server)
{
	for(std::map<std::string, location *>::iterator it = server->Locations.begin(); it != server->Locations.end(); it++) {
		delete it->second;
	}
	delete server;
}

bool validateDataBlock(serverConfig *server)
{
	std::map<std::string, location *>::iterator it = server->Locations.begin();
	if (server->getPorts().size() == 0) {
		std::cout << "Error: port number not found" << std::endl;
		return false;
	}
	for (std::map<std::string, location *>::iterator it = server->Locations.begin(); it != server->Locations.end(); it++) {

		if ((it->second->rootPath.length() == 0)) {
			if (server->getRootPath().length() == 0) {
				std::cout << "Error: there is no root path defined location " << it->first << std::endl;
				return false;
			}
			it->second->rootPath = server->getRootPath();
		}
		if (it->second->allowMethods.empty()) {
			if (server->getAllowedMethods().empty()) {
				std::cerr << "Error: location " << it->first << " does not contain allowed methods" << std::endl;
			}
			it->second->allowMethods = server->getAllowedMethods();
		}
		if (it->second->autoindex == false) {
			it->second->autoindex = server->getAutoIndexFlag();
		}
		if (it->second->indexFiles.size() == 0) {
			it->second->indexFiles = server->getIndexFiles();
		}
		if (it->second->clientMaxSizeBody == 0) {
			it->second->clientMaxSizeBody = server->getMaxBodySize();
		}
	}
	return true;
}

void config::startEvaluation(parser& p)
{
    int i = 0;
	std::set<int> checkPortDuplicate;

    while (p.peekNode(i) && p.peekNode(i)->type == SERVER_NODE)
    {
		serverConfig *server = new serverConfig();
        std::vector<AstNode *> children = p.peekNode(i)->children;
        customSever(children, server);
        if (this->error || !validateDataBlock(server)) {
			clearServer(server);
			this->error = true;
            break ;
        }
		std::vector<int> ports = server->getPorts();
		for (size_t i = 0; i < server->getPorts().size(); i++)
		{
			if (checkPortDuplicate.count(ports[i])) {
				std::cout << "Error: virtual host not implemented, dont use the same port number more then one server" << std::endl;
				this->error = true;
				clearServer(server);
				break ;
			}
			else {
				checkPortDuplicate.insert(ports[i]);
			}
		}
        this->servers.push_back(server);
        i++;
    }
}

void config::buildServersConfig( void )
{
    tokenizer tok(filename);
	
    if (tok.checkTokenizeError()) {
		this->error = true;
        return ;
    }
    tok.tokenizerStart();
    if (tok.getTokensSize() == 0) {
		std::cout << "Error: empty or invalid configuration file" << std::endl;
        this->error = true;
        return ;
    }

    parser p(tok);
    p.startParser();
    if (p.checkErrorParse()) {
		this->error = true;
        return ;
    }
    startEvaluation(p);
}

void config::printServer()
{
	size_t k = 0;
	while (k < servers.size()) {
		for (std::map<std::string, location *>::iterator it = servers[k]->Locations.begin(); it != servers[k]->Locations.end(); it++) {
			std::cout << "location data*******************************************************\n\n" << std::endl;
			std::cout << "location key == " << it->first << std::endl;
			std::cout << "clientMaxSizeBody: " << it->second->clientMaxSizeBody << std::endl;
			std::cout << "autoindex: " << it->second->autoindex << std::endl;
			std::cout << "allowed methods: ";
			for (std::set<std::string>::iterator setit = it->second->allowMethods.begin(); setit != it->second->allowMethods.end(); setit++) {
				std::cout << *setit << " ; ";
			}
			std::cout << std::endl;
			std::cout << "cgi pass : " << it->second->cgi_pass << std::endl;
			std::cout << "cgi extention : ";
			for (size_t j = 0; j < it->second->cgi_extension.size(); j++) {
				std::cout << it->second->cgi_extension[j] << " ; ";
			}
			std::cout << std::endl;
			std::cout << "root path == " << it->second->rootPath << std::endl;
			std::cout << "index files == ";
			for (size_t j = 0; j < it->second->indexFiles.size(); j++) {
				std::cout << it->second->indexFiles[j] << " ; ";
			}
			std::cout << std::endl;
		}
		std::cout << "locations finish :: =============================" << std::endl;
		std::cout << "finish server  : *******************************************************\n\n" << std::endl;
		std::cout << std::endl;
		k++;
	}

}