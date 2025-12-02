#include "../includes/socketsManager.hpp"

socketsManager::socketsManager(serverConfig *conf, int fd, ServerManager *ptr): 
serverConf(conf), socketFd(fd), ptr(ptr) {}

socketsManager::~socketsManager() {}