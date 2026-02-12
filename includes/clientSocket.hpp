#ifndef CLIENTSOCKET_HPP
#define CLIENTSOCKET_HPP

#include "./socketsManager.hpp"
#include "../HandleRequests/TransactionManager.hpp"

#define MAX_BUFFER_SIZE 8192

enum ClientState {
    READING_REQUEST,
    WRITING_RESPONSE,
    ERROR_RESP
};

class ClientSocket: public socketsManager {
private:

    ClientState         state;
    TransactionManager  *transactionMgr;

    void readingAndProcessingRequest( void );
    void continueWriting();
    void errorResponse( void );

public:

	ClientSocket(int fd ,serverConfig *conf);
    ~ClientSocket();
    void    handleEvent();

};

#endif
