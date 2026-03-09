#include "config.hpp"

config::config(std::string FileName) : filename(FileName), error(false) {}

config::~config()
{
	for (size_t i = 0; i < servers.size(); i++)
	{
		for (std::map<std::string, location *>::iterator it = servers[i]->Locations.begin(); it != servers[i]->Locations.end(); it++)
		{
			delete it->second;
		}
		if (servers[i]->getSessionManager() != NULL)
			delete servers[i]->getSessionManager();
		delete servers[i];
	}
}

serverConfig *config::getSerevrConfig(int index)
{
	return this->servers[index];
}

SessionManager *serverConfig::getSessionManager() { return this->sessionManager; }
void serverConfig::setSessionManager(SessionManager *sessionManager) { this->sessionManager = sessionManager; }
std::map<int, std::string> &serverConfig::getErrorPages() { return this->errorPages; }
std::vector<int> &serverConfig::getPorts() { return this->Port; }
std::vector<std::string> &serverConfig::getIps() { return this->ip; };
size_t serverConfig::getMaxBodySize() { return clientMaxSizeBody; }
std::vector<std::string> &serverConfig::getServerNames()
{
	return this->ServerNames;
}
std::string &serverConfig::getRootPath() { return this->rootPath; }
std::vector<std::string> &serverConfig::getIndexFiles() { return this->indexFiles; }
bool serverConfig::getAutoIndexFlag() { return this->autoindex; }
std::set<std::string> &serverConfig::getAllowedMethods() { return this->allowMethods; }
std::pair<int, std::string> &serverConfig::getRedirection() { return this->redirection; }
void serverConfig::setPort(int port) { this->Port.push_back(port); }
void serverConfig::setMaxBodySize(size_t clientBody) { this->clientMaxSizeBody = clientBody; }
void serverConfig::setServerName(std::string serverName) {this->ServerNames.push_back(serverName); }
void serverConfig::setRootPath(std::string path) { this->rootPath = path; }
void serverConfig::setIndexFile(std::string indexFile) { this->indexFiles.push_back(indexFile); }
void serverConfig::setAutoIndexFlag() { this->autoindex = true; }
void serverConfig::setAllowedMethod(std::string method) { this->allowMethods.insert(method); }
void serverConfig::setRedirection(int code, std::string url)
{
	this->redirection.first = code;
	this->redirection.second = url;
}
void serverConfig::setIp(std::string ip) { this->ip.push_back(ip); }
void serverConfig::setErrorPage(int number, std::string path) { this->errorPages[number] = path; }

bool config::CheckParse()
{
	return this->error;
}

int config::ServersNumber()
{
	return servers.size();
}

void config::customDataLocation(AstNode *node, location *loc)
{
	if (node->name == "root")
	{
		if (node->args.size() != 1)
		{
			std::cout << "Error : multiple roots declared at a line" << std::endl;
			this->error = true;
			return;
		}
		else
		{
			loc->rootPath = node->args[0];
		}
	}
	else if (node->name == "client_max_body_size")
	{
		if (node->args.size() != 1) {
			std::cout << "Error : invalide initialization of client max size body" << std::endl;
			this->error = true;
			return;
		}
		for (size_t j = 0; j < node->args[0].length(); j++)
		{
			if (!isdigit(node->args[0][j]))
			{
				std::cout << "Error : invalide client max size body number detected" << "{" << node->args[0] << "}" << std::endl;
				this->error = true;
				return;
			}
		}
		loc->clientMaxSizeBody = std::strtoul((node->args[0].c_str()), NULL, 10);
	}
	else if (node->name == "index")
	{
		for (size_t i = 0; i < node->args.size(); i++)
		{
			loc->indexFiles.push_back(node->args[i]);
		}
	}
	else if (node->name == "autoindex")
	{
		if (node->args.size() != 1) {
			std::cout << "Error: invalid initialization of autoindex it must be like {autoindex off/on}" << std::endl;
			this->error = true;
			return ;
		}
		if (node->args[0] == "on")
		{
			loc->autoindex = true;
		}
		else if (node->args[0] != "off")
		{
			std::cout << "Warning: Unknown word " << node->args[0] << " auto index will treat as default (off)" << std::endl;
		}
	}
	else if (node->name == "methods")
	{
		for (size_t i = 0; i < node->args.size(); i++)
		{
			if (node->args[i] == "GET" || node->args[i] == "POST" || node->args[i] == "DELETE")
			{
				loc->allowMethods.insert(node->args[i]);
			}
			else
			{
				std::cout << "Error : Unknown method " << node->args[i] << std::endl;
				this->error = true;
				return;
			}
		}
	}
	else if (node->name == "cgi_extension")
	{
		for (size_t i = 0; i < node->args.size(); i++)
		{
			loc->cgi_extension.push_back(node->args[i]);
		}
	}
	else if (node->name == "cgi_pass")
	{
		if (node->args.size() != 1)
		{
			std::cout << "Warning : multiple values of cgi_pass; the first will be used" << std::endl;
		}
		loc->cgi_pass = node->args[0];
	}
	else if (node->name == "return")
	{
		for (size_t i = 0; i < node->args[0].length(); i++)
		{
			if (!isdigit(node->args[0][i]))
			{
				std::cout << "Error : invalide redirection code " << "{" << node->args[0] << "}" << std::endl;
				this->error = true;
				return;
			}
		}
		loc->redirection.first = std::atoi((node->args[0].c_str()));
		if (loc->redirection.first == 301 || loc->redirection.first == 302)
		{
			if (node->args[1].length() > 0)
			{
				loc->redirection.second = node->args[1];
			}
			else
			{
				std::cout << "Error: redirection number " << loc->redirection.first << " must contain URL" << std::endl;
				this->error = true;
				return;
			}
		}
		else
		{
			std::cout << "Error: number of redirection not handled, must be {301 or 302}" << std::endl;
			this->error = true;
			return;
		}
	}
	else if (node->name == "upload_store")
	{
		if (node->args.size() != 1)
		{
			std::cout << "Error: multiple args in upload store" << std::endl;
			this->error = true;
			return;
		}
		loc->upload_store = node->args[0];
	}
	else
	{
		std::cout << "Warning: Unknown directive '" << node->name << "' " << "it will ignored" << std::endl;
	}
}

void config::customLocation(AstNode *LocationRoot, location *loc)
{
	loc->key = LocationRoot->args[0];
	std::vector<AstNode *> children = LocationRoot->children;
	for (size_t i = 0; i < children.size(); i++)
	{
		customDataLocation(children[i], loc);
		if (this->error)
		{
			break;
		}
	}
}

void config::customDataServer(AstNode *node, serverConfig *server)
{
	if (node->name == "listen")
	{
		for (size_t i = 0; i < node->args.size(); i++)
		{
			std::string ipPort = node->args[i];
			size_t pos = ipPort.find(":");
			if (pos == std::string::npos)
			{
				std::cout << "Error : Invalid IP:Port format, expected format IP:Port" << std::endl;
				this->error = true;
				return;
			}
			else
			{
				server->setIp(ipPort.substr(0, pos));
				node->args[i] = ipPort.substr(pos + 1);
			}
			if (node->args[i].length() > 5)
			{
				std::cout << "Error : Invalide port number " << node->args[i] << std::endl;
				this->error = true;
				return;
			}
			for (size_t j = 0; j < node->args[i].length(); j++)
			{
				if (!isdigit(node->args[i][j]))
				{
					std::cout << "Error : invalide port number detected" << std::endl;
					this->error = true;
					return;
				}
			}
			int port = std::atoi((node->args[i].c_str()));
			if (port <= 0 || port > 65535)
			{
				std::cout << "Error the port " << port << " outside the range 1 - 65535" << std::endl;
				this->error = true;
				return;
			}
			server->setPort(std::atoi((node->args[i].c_str())));
		}
	}
	else if (node->name == "client_max_body_size")
	{
		if (node->args.size() != 1) {
			std::cout << "Error : invalide initialization of client max size body" << std::endl;
			this->error = true;
			return;
		}
		for (size_t j = 0; j < node->args[0].length(); j++)
		{
			if (!isdigit(node->args[0][j]))
			{
				std::cout << "Error : invalide client max size body number detected" << std::endl;
				this->error = true;
				return;
			}
		}
		server->setMaxBodySize(std::strtoul((node->args[0].c_str()), NULL, 10));
	}
	else if (node->name == "server_name")
	{
		for (size_t i = 0; i < node->args.size(); i++)
		{
			server->setServerName(node->args[i]);
		}
	}
	else if (node->name == "root")
	{
		if (node->args.size() == 1)
		{
			server->setRootPath(node->args[0]);
		}
		else if (node->args.size() > 1)
		{
			std::cout << "Error : multiple roots declared invalid" << std::endl;
			this->error = true;
			return ;
		}
	}
	else if (node->name == "index")
	{
		for (size_t i = 0; i < node->args.size(); i++)
		{
			server->setIndexFile(node->args[i]);
		}
	}
	else if (node->name == "autoindex")
	{
		if (node->args.size() != 1) {
			std::cout << "Error: invalid initialization of autoindex it must be like {autoindex off/on}" << std::endl;
			this->error = true;
			return ;
		}
		if (node->args[0] == "on")
			server->setAutoIndexFlag();
		else if (node->args[0] != "off")
		{
			std::cout << "Warning: Unknown word " << node->args[0] << " auto index will treat as default (off)" << std::endl;
		}
	}
	else if (node->name == "methods")
	{
		for (size_t i = 0; i < node->args.size(); i++)
		{
			if (node->args[i] == "GET" || node->args[i] == "POST" || node->args[i] == "DELETE")
			{
				server->setAllowedMethod(node->args[i]);
			}
			else
			{
				std::cout << "Error : Unknown method " << node->args[i] << std::endl;
				this->error = true;
				return;
			}
		}
	}
	else if (node->name == "return")
	{
		std::string url;
		int code;
		for (size_t i = 0; i < node->args[0].length(); i++)
		{
			if (!isdigit(node->args[0][i]))
			{
				std::cout << "Error : invalide redirection code " << "{" << node->args[0] << "}" << std::endl;
				this->error = true;
				return;
			}
		}
		code = std::atoi((node->args[0].c_str()));
		if (code == 301 || code == 302)
		{
			if (node->args[1].length() > 0)
			{
				url = node->args[1];
			}
			else
			{
				std::cout << "Error: redirection number " << code << " must contain URL" << std::endl;
				this->error = true;
				return;
			}
		}
		else
		{
			std::cout << "Error: number of redirection not handled, must be {301 or 302}" << std::endl;
			this->error = true;
			return;
		}
		server->setRedirection(code, url);
	}
	else if (node->name == "error_pages")
	{
		int last = node->args.size() - 1;
		if (last <= 0)
		{
			std::cout << "Warning: invalid initialize of error page " << std::endl;
		}
		std::string errorPage = node->args[last];
		for (size_t i = 0; i < node->args.size() - 1; i++)
		{
			int errorNumber = std::atoi((node->args[i].c_str()));
			if (
				errorNumber == 400 ||
				errorNumber == 403 ||
				errorNumber == 404 ||
				errorNumber == 405 ||
				errorNumber == 413 ||
				errorNumber == 500 ||
				errorNumber == 501 ||
				errorNumber == 503)
			{
				server->setErrorPage(errorNumber, errorPage);
			}
			else
			{
				std::cout << "Warning : invalide error page number " << errorNumber << " it will ignored" << std::endl;
			}
		}
	}
	else
	{
		std::cout << "Warning: Unknown directive '" << node->name << "' " << "it will ignored in server block" << std::endl;
	}
}

void config::customSever(std::vector<AstNode *> root, serverConfig *server)
{
	for (size_t i = 0; i < root.size(); i++)
	{
		if (root[i]->type == LOCATION_NODE)
		{
			location *loc = new location();
			customLocation(root[i], loc);
			if (this->error)
			{
				delete loc;
				break;
			}
			server->Locations.insert(std::make_pair(loc->key, loc));
		}
		else
		{
			customDataServer(root[i], server);
			if (this->error)
			{
				break;
			}
		}
	}
}

void clearServer(serverConfig *server)
{
	for (std::map<std::string, location *>::iterator it = server->Locations.begin(); it != server->Locations.end(); it++)
	{
		delete it->second;
	}
	delete server;
}

bool validateDataBlock(serverConfig *server)
{
	if (server->getPorts().size() == 0)
	{
		std::cout << "Error: port number not found" << std::endl;
		return false;
	}
	for (std::map<std::string, location *>::iterator it = server->Locations.begin(); it != server->Locations.end(); it++)
	{

		if ((it->second->rootPath.length() == 0))
		{
			if (server->getRootPath().length() == 0)
			{
				std::cout << "Error: there is no root path defined location " << it->first << std::endl;
				return false;
			}
			it->second->rootPath = server->getRootPath();
		}
		if (it->second->allowMethods.empty())
		{
			if (server->getAllowedMethods().empty())
			{
				std::cerr << "Error: location " << it->first << " does not contain allowed methods" << std::endl;
				return false;
			}
			it->second->allowMethods = server->getAllowedMethods();
		}
		if (it->second->autoindex == false)
		{
			it->second->autoindex = server->getAutoIndexFlag();
		}
		if (it->second->indexFiles.size() == 0)
		{
			it->second->indexFiles = server->getIndexFiles();
		}
		if (it->second->clientMaxSizeBody == 0)
		{
			it->second->clientMaxSizeBody = server->getMaxBodySize();
		}
	}
	return true;
}

void config::startEvaluation(parser &p)
{
	int i = 0;
	std::set<int> checkPortDuplicate;

	while (p.peekNode(i) && p.peekNode(i)->type == SERVER_NODE)
	{
		serverConfig *server = new serverConfig();
		std::vector<AstNode *> children = p.peekNode(i)->children;
		customSever(children, server);
		if (this->error || !validateDataBlock(server))
		{
			clearServer(server);
			this->error = true;
			break;
		}
		std::vector<int> &ports = server->getPorts();
		for (size_t i = 0; i < ports.size(); i++)
		{
			if (checkPortDuplicate.count(ports[i]))
			{
				std::cout << "Error: virtual host not implemented, dont use the same port number more then one server" << std::endl;
				this->error = true;
				clearServer(server);
				return;
			}
			else
			{
				checkPortDuplicate.insert(ports[i]);
			}
		}
		this->servers.push_back(server);
		i++;
	}
}

void config::buildServersConfig(void)
{
	tokenizer tok(filename);

	if (tok.checkTokenizeError())
	{
		this->error = true;
		return;
	}
	tok.tokenizerStart();
	if (tok.getTokensSize() == 0)
	{
		std::cout << "Error: empty or invalid configuration file" << std::endl;
		this->error = true;
		return;
	}

	parser p(tok);
	p.startParser();
	if (p.checkErrorParse())
	{
		this->error = true;
		return;
	}
	startEvaluation(p);
}

// --------------------------------------------------------------------------- print-------
void config::printServer()
{
	size_t k = 0;
	while (k < servers.size())
	{
		for (std::map<std::string, location *>::iterator it = servers[k]->Locations.begin(); it != servers[k]->Locations.end(); it++)
		{
			std::cout << "location data*******************************************************\n\n"
					  << std::endl;
			std::cout << "location key == " << it->first << std::endl;
			std::cout << "return == " << it->second->redirection.first << " " << it->second->redirection.second << std::endl;
			std::cout << "clientMaxSizeBody: " << it->second->clientMaxSizeBody << std::endl;
			std::cout << "autoindex: " << it->second->autoindex << std::endl;
			std::cout << "upload store: " << it->second->upload_store << std::endl;
			std::cout << "allowed methods: ";
			for (std::set<std::string>::iterator setit = it->second->allowMethods.begin(); setit != it->second->allowMethods.end(); setit++)
			{
				std::cout << *setit << " ; ";
			}
			std::cout << std::endl;
			std::cout << "cgi pass : " << it->second->cgi_pass << std::endl;
			std::cout << "cgi extention : ";
			for (size_t j = 0; j < it->second->cgi_extension.size(); j++)
			{
				std::cout << it->second->cgi_extension[j] << " ; ";
			}
			std::cout << std::endl;
			std::cout << "root path == " << it->second->rootPath << std::endl;
			std::cout << "index files == ";
			for (size_t j = 0; j < it->second->indexFiles.size(); j++)
			{
				std::cout << it->second->indexFiles[j] << " ; ";
			}
			std::cout << std::endl;
		}
		std::cout << "locations finish :: =============================" << std::endl;
		std::cout << "finish server  : *******************************************************\n\n"
				  << std::endl;
		std::cout << std::endl;
		k++;
	}
}