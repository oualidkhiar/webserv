#include "socketsManager.hpp"

socketsManager::socketsManager(serverConfig *conf, int fd): 
serverConf(conf), socketFd(fd), action(NO_ACTION) {}

socketsManager::~socketsManager() {}