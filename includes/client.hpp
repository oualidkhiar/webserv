#ifndef CLIENT_HPP
#define CLIENT_HPP

#include "./socketsManager.hpp"


// enum ClientState {

// };

class ClientSocket: public socketsManager {
private:

    bool		    RequestIsFull;
    bool		    ResponseIsComplet;
    std::string	    ReceiveBuffer;
    std::string	    ResponseBuffer;

public:
	ClientSocket(serverConfig *conf, int fd);
    void handleSocketsAction();
    // int handleRequest( void );
    // int handleResponse( void );

};

#endif