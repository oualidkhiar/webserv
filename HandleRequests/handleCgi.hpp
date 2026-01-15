#pragma once
#include "HttpRequest.hpp"
#include "HttpResponse.hpp"
#include "../includes/config.hpp"

class Cgi {
private:
    HttpRequest& request;
public:
    Cgi(HttpRequest& request): request(request) {

    }
    ~Cgi() {}

    void executeCgi( void )
    {
        
    }
};



            //   cgi request example


//              POST /cgi-bin/login.cgi?debug=true HTTP/1.1     |
//              Host: example.com:8080                          |   
//              User-Agent: curl/8.4.0                          |
//              Accept: */*                                     |
//              Content-Type: application/x-www-form-urlencoded |
//              Content-Length: 27                              |
//              Cookie: session=abc123                          |
//                                                              |                                                               
//              username=walid&pwd=42                           |                                                               
//                                                              |


