#include "HttpRequest.hpp"
#include "RequestParser.hpp"
#include <iostream>
#include <map>

void cout_request(HttpRequest &request)
{
    std::string types[3] = {"DELETE", "POST", "GET"};
    std::string status[5] = {"READING_REQUEST_LINE",
                             "READ_HEADER",
                             "READ_BODY",
                             "FINISHED",
                             "ERROR"};
    std::map<std::string, std::string>
        headers;
    headers = request.getHeaders();

    std::cout << "URI = '" << request.getUri() <<"'"<< std::endl;
    std::cout << "Type = " << types[request.getType()] << std::endl;
    std::cout << "Status = " << status[request.getStatus()] << std::endl;
    std::map<std::string, std::string>::const_iterator it;
    for (it = headers.begin() ; it != headers.end() ; ++it)
    {
        std::cout<<it->first<<" : "<<it->second<<std::endl;
    }
}

int main()
{
    std::string raw_get =
        "GET / HTTP/1.1\r\n"
        "Host: localhost:8080\r\n"
        "User-Agent: ::TestingClient/1.0\r\n"
        "Accept: */*\r\n"
        "\r\n";
    HttpRequest request(raw_get);
    RequestParser request_parser;
    while (request.getStatus() != FINISHED)
        request_parser.create_request(request);
    std::cout << "REQEUST FINISHED" << std::endl;
    cout_request(request);
}