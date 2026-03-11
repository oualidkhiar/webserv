#ifndef CLIENTSOCKET_HPP
#define CLIENTSOCKET_HPP

#include "./socketsManager.hpp"
#include "./TransactionManager.hpp"

enum ClientState {
    READING_REQUEST,
    WRITING_RESPONSE,
    ERROR_RESP
};

class ClientSocket: public socketsManager {
private:

    ClientState         state;
    TransactionManager  *transactionMgr;
    int                  consecutive_failures_for_read;
    int                  consecutive_failures_for_write;

    void readingAndProcessingRequest( void );
    void sendingResponse();
    void ErrorParseRequest( void );

public:

	ClientSocket(int fd ,serverConfig *conf);
    ~ClientSocket();

    // overridden functions
    void    handleEvent();
    bool    isTimeOut();

};

#endif
