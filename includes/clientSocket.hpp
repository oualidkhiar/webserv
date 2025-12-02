#ifndef CLIENTSOCKET_HPP
#define CLIENTSOCKET_HPP

#include "./socketsManager.hpp"


enum ClientState {
    READING_REQUEST,
    PROCESSING,
    WRITING_RESPONSE,
    CLOSED
};


class ClientSocket: public socketsManager {
private:

    ClientState     state;
    std::string	    ReceiveBuffer;
    std::string	    ResponseBuffer;

public:
	ClientSocket(int fd ,serverConfig *conf, ServerManager *ptr);
    void handleEvent();
    int handleRequest( void );
    int handleResponse( void );

};

#endif