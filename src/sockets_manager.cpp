#include "../includes/socketsManager.hpp"

socketsManager::socketsManager(serverConfig *conf, ServerManager *ptr, int fd): 
serverConf(conf), ptr(ptr), socketFd(fd) {}

socketsManager::~socketsManager() {}