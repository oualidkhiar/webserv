
#ifndef LISTENINGSOCKET_HPP
#define LISTENINGSOCKET_HPP

#include "./config.hpp"
#include "./constent.hpp"
#include "./socketsManager.hpp"

class ListeningSocket: public socketsManager {
public:

    ListeningSocket(serverConfig *conf, int fd, ServerManager *ptr);
    ~ListeningSocket();

    void handleEvent();

};

#endif