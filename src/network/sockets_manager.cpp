#include "socketsManager.hpp"

socketsManager::socketsManager(serverConfig *conf, int fd): 
serverConf(conf), socketFd(fd), action(NO_ACTION) {}

socketsManager::~socketsManager() {}

Action socketsManager::getAction() {return this->action;}
void socketsManager::setActionNone( void ) {this->action = NO_ACTION;}
int socketsManager::getFd() {return socketFd;}
std::vector<socketsManager *>& socketsManager::getNewClient() {return this->newClientFds;}
void socketsManager::clearVector() {newClientFds.clear();}
void socketsManager::updateTimeInteraction() {this->lastTimeInteraction = time(NULL);}