#include "../includes/config.hpp"

config::config(std::string FileName): filename(FileName), index(0), error(false) {}

config::~config()
{
    for (int i = 0; i < servers.size(); i++) {
		for (std::unordered_map<std::string, location *>::iterator it = servers[i]->Locations.begin(); it != servers[i]->Locations.end(); it++) {
			delete it->second;
		}
		delete servers[i];
    }
	std::cout << "destructor called " << std::endl;
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

void config::customDataLocation(AstNode *node, location *loc, int locationNumber)
{
	if (node->name == "root") {
		if (node->args.size() != 1) {
			std::cout << "Error : multiple roots declared at a line, location number " << locationNumber+1 << std::endl;
			this->error = true;
			return ;
		}
		else {
			loc->rootPath = node->args[0];
		}
	}
	else if (node->name == "client_max_body_size" && node->args.size() == 1) {
		for (int j = 0; j < node->args[0].length(); j++) {
			if (!isdigit(node->args[0][j])) {
				std::cout << "Error : invalide client max size body number detected" << "{" << node->args[0] << "}, location number " << locationNumber+1 << std::endl;
				this->error = true;
				return ;
			}
		}
		loc->clientMaxSizeBody = atoi((node->args[0].c_str()));
	}
	else if (node->name == "index") {
		for (int i = 0; i < node->args.size(); i++) {
			loc->indexFiles.push_back(node->args[i]);
		}
	}
	else if (node->name == "autoindex") {
		if (node->args[0] == "on") {
			loc->autoindex = true;
		}
		else if (node->args[0] != "off") { 
			std::cout << "Warning: Unknown word " << node->args[0] << " auto index will treat as default (off)" << "location number "<< locationNumber+1 << std::endl; 
		}
	}
	else if (node->name == "methods") {
		for (int i = 0; i < node->args.size(); i++) {
			if (node->args[i] == "GET" || node->args[i] == "POST" || node->args[i] == "DELETE") {
				loc->allowMethods.insert(node->args[i]);
			}
			else {
				std::cout << "Error : Unknown method " << node->args[i] << " in location number " << locationNumber+1 << std::endl;
				this->error = true;
				return ;
			}
		}
	}
	else if (node->name == "cgi_extension") {
		for (int i = 0; i < node->args.size(); i++) {
			loc->cgi_extension.push_back(node->args[i]);
		}
	}
	else if (node->name == "cgi_pass") {
		if (node->args.size() != 1) {
			std::cout << "Warning : multiple values of cgi_pass; the first will be used, location " << locationNumber+1 << std::endl;
		}
		loc->cgi_pass = node->args[0];
	}
	else {
		std::cout << "Warning: Unknown directive '" << node->name << "' " << "it will ignored, location number " << locationNumber+1 << std::endl;
	}
}

void config::customLocation(AstNode *LocationRoot, location *loc)
{
	static int LocNumber;
	loc->key = LocationRoot->args[0];
	std::vector<AstNode *> children = LocationRoot->children;
	for (int i = 0; i < children.size(); i++) {
		customDataLocation(children[i], loc, LocNumber);
		if (this->error) {
			break ;
		}
	}
	LocNumber++;
}

void config::customDataServer(AstNode *node, serverConfig *server)
{
	if (node->name == "listen") {
		for (int i = 0; i < node->args.size(); i++) {
			if (node->args[i].length() > 5) {
				std::cout << "Error : Invalide port number " << node->args[i] << std::endl;
				this->error = true;
				return ;
			}
			for (int j = 0; j < node->args[i].length(); j++) {
				if (!isdigit(node->args[i][j])) {
					std::cout << "Error : invalide port number detected" << std::endl;
					this->error = true;
					return ;
				}
			}
			server->Port.push_back(atoi((node->args[i].c_str())));
			if (server->Port[i] <= 0 || server->Port[i] > 65535) {
				std::cout << "Error the port " << server->Port[i] << " outside the range 1 - 65535" << std::endl;
				this->error = true;
				return ;
			}
		}
	}
	else if (node->name == "client_max_body_size" && node->args.size() == 1) {
		for (int j = 0; j < node->args[0].length(); j++) {
			if (!isdigit(node->args[0][j])) {
				std::cout << "Error : invalide client max size body number detected" << std::endl;
				this->error = true;
				return ;
			}
		}
		server->clientMaxSizeBody = atoi((node->args[0].c_str()));
	}
	else if (node->name == "server_name") {
		for (int i = 0; i < node->args.size(); i++) {
			server->ServerNames.push_back(node->args[i]);
		}
	}
	else if (node->name == "root") {
		if (node->args.size() == 1) {
			server->rootPath = node->args[0];
		}
		else if (node->args.size() > 1) {
			std::cout << "Error : multiple roots declared invalid" << std::endl;
		}
	}
	else if (node->name == "error_page") {
		int last = node->args.size() - 1;
		if (last <= 0) {
			std::cout << "Error : invalid initialize of error page " << std::endl;
		}
		std::string errorPage = node->args[last];
		for (int i = 0; i < node->args.size() - 1; i++) {
			int errorNumber = atoi((node->args[i].c_str()));
			if (
				errorNumber == 400 || 
				errorNumber == 403 || 
				errorNumber == 404 || 
				errorNumber == 405 || 
				errorNumber == 413 || 
				errorNumber == 500 || 
				errorNumber == 501 || 
				errorNumber == 503
			)
			{
				server->errorPage[errorNumber] = errorPage;
			}
			else {
				std::cout << "Error : invalide error page number " << errorNumber << std::endl;
				this->error = true;
				return ;
			}
		}
	}
	else if (node->name == "index") {
		for (int i = 0; i < node->args.size(); i++) {
			server->indexFiles.push_back(node->args[i]);
		}
	}
	else if (node->name == "autoindex") {
		if (node->args[0] == "on")
			server->autoindex = true;
	}
	else {
		std::cout << "Warning: Unknown directive '" << node->name << "' " << "it will ignored in server block" << std::endl;
	}
}

void config::customSever(std::vector<AstNode *> root, serverConfig *server)
{
    for (int i = 0; i < root.size(); i++) {
        if (root[i]->type == LOCATION_NODE) {
            location *loc = new location();
            customLocation(root[i], loc);
            if (this->error) {
                delete loc;
                break ;
            }
            server->Locations.insert({loc->key, loc});
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
	for(auto it = server->Locations.begin(); it != server->Locations.end(); it++) {
		delete it->second;
	}
	delete server;
}

bool validateDataBlock(serverConfig *server) 
{
	if (server->Port.size() == 0 || server->rootPath.length() == 0) {
		return false;
	}
	if (server->indexFiles.size() == 0) {
		// server->indexFiles.push_back("index.html") TODO: use default
	}
	if (server->clientMaxSizeBody == -1) {
		server->clientMaxSizeBody = DEFAULT_SIZE;
	}
	if (server->ServerNames.size() == 0) {
		//TODO : use default server name
	}
	if (server->errorPage.size() == 0) {
		// assign the necisser error pages for exaple 404 500 
	}
	return true;
}

void config::startEvaluation(parser& p)
{
    int i = 0;	
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
	int k = 0;
	while (k < servers.size()) {
		
		std::cout << "Server block data N" << k << ": --------------------------------------------" << std::endl << std::endl; 
		std::cout << "clientMaxSizeBody: " << this->servers[k]->clientMaxSizeBody << std::endl;

		std::cout << "rootPath: " << this->servers[k]->rootPath << std::endl;

		for (auto it = servers[k]->errorPage.begin(); it != servers[k]->errorPage.end(); it++) {
			std::cout << "error number: " << it->first << " error page: " << it->second << std::endl;
		}

		for (int i = 0; i < servers[k]->ServerNames.size(); i++) {
			std::cout << "server name " << i << ": " <<  servers[k]->ServerNames[i] << std::endl;
		}

		for (int i = 0; i < servers[k]->indexFiles.size(); i++) {
			std::cout << "index file " << i << ": " <<  servers[k]->indexFiles[i] << std::endl;
		}

		for (int i = 0; i < servers[k]->Port.size(); i++) {
			std::cout << "port " << i << ": " <<  servers[k]->Port[i] << std::endl;
		}

		std::cout << "locations :----------------------------------" << std::endl;

		for (auto it = servers[k]->Locations.begin(); it != servers[k]->Locations.end(); it++) {
			std::cout << "location key == " << it->first << std::endl;
			std::cout << "location data ::: ::: :::: :::  :::: ::: " << std::endl;
			std::cout << "clientMaxSizeBody: " << it->second->clientMaxSizeBody << std::endl;
			std::cout << "autoindex: " << it->second->autoindex << std::endl;
			std::cout << "allowed methods: ";

			for (auto setit = it->second->allowMethods.begin(); setit != it->second->allowMethods.end(); setit++) {
				std::cout << *setit << " ; ";
			}
			std::cout << std::endl;
			std::cout << "cgi pass : " << it->second->cgi_pass << std::endl;
			std::cout << "cgi extention : ";
			for (int j = 0; j < it->second->cgi_extension.size(); j++) {
				std::cout << it->second->cgi_extension[j] << " ; ";
			}
			std::cout << std::endl;
			std::cout << "root path == " << it->second->rootPath << std::endl;
			std::cout << "index files == ";
			for (int j = 0; j < it->second->indexFiles.size(); j++) {
				std::cout << it->second->indexFiles[j] << " ; ";
			}
			std::cout << std::endl;
		}
		std::cout << "locations finish :: =============================" << std::endl;
		std::cout << "finish server  : --------------------------------------------------------" << std::endl;
		std::cout << std::endl;
		k++;
	}

}