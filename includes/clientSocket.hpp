#ifndef CLIENTSOCKET_HPP
#define CLIENTSOCKET_HPP

#include "./socketsManager.hpp"


enum ClientState {
    READING_REQUEST,
    PROCESSING,
    WRITING_RESPONSE,
    CLOSED
};


class requestHandler {

};

class ClientSocket: public socketsManager {
private:

    ClientState     state;
    std::string	    ReceiveBuffer;
    std::string	    ResponseBuffer;

    requestHandler  request;

    void continueReading( void );

public:

	ClientSocket(int fd ,serverConfig *conf, ServerManager *ptr);
    void handleEvent();
    void handleRequest( void );
    void handleResponse( void );

};

#endif
