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
    char	        *ReceiveBuffer;
    char	        *ResponseBuffer;

    void readingAndProcessingRequest( void );
    void continueWriting(std::string& res);

public:

	ClientSocket(int fd ,serverConfig *conf, ServerManager *ptr);
    void    handleEvent();

};

#endif
