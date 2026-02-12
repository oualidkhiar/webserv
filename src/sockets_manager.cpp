#include "../includes/socketsManager.hpp"

socketsManager::socketsManager(serverConfig *conf, int fd): 
serverConf(conf), socketFd(fd) {}

socketsManager::~socketsManager() {}