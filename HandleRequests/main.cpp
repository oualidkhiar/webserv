#include "HttpRequest.hpp"
#include "RequestParser.hpp"
#include <iostream>
#include <map>
#include "HttpResponse.hpp"
#include "TransactionManager.hpp"

size_t ft_strlen(unsigned char *s)
{
    int i = 0;
    while (s[i])
        i++;
    return (i);
}

void cout_response(HttpResponse &response)
{
    // std::cout << "status = " << response.getStatus() << std::endl;

    //  response.printHeaders();
    response.printBody();
}
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

    std::cout << "URI = '" << request.getUri() << "'" << std::endl;
    std::cout << "Type = " << types[request.getType()] << std::endl;
    std::cout << "Status = " << status[request.getStatus()] << std::endl;
    std::map<std::string, std::string>::const_iterator it;
    for (it = headers.begin(); it != headers.end(); ++it)
    {
        std::cout << it->first << " : " << it->second << std::endl;
    }
    if (request.getType() == POST)
    {
        request.printBody();
    }
}

int main()
{

    unsigned char fixedPost[500] =
        "POST /api/users/create HTTP/1.1\r\n"
        "Host: localhost:8080\r\n"
        "Content-Type: application/json\r\n"
        "Content-Length: 70\r\n" // Total bytes of the body string
        "User-Agent: TestingClient/1.0\r\n"
        "\r\n" // End of Headers
        "{\"username\": \"jdoe\", \"email\": \"jane.doe@example.com\", \"role\": \"admin\"}";
    TransactionManager manager;
    manager.appendToRequest(fixedPost, ft_strlen(fixedPost));
    std::pair<unsigned char * , size_t> response  = manager.getResponse();
    write(1,response.first , response.second);

    // HttpRequest request;
    // request.appendRequestData(fixedPost, ft_strlen(fixedPost));
    // RequestParser request_parser;
    // while (request.getStatus() != FINISHED)
    // {
    //     request_parser.create_request(request);
    // }
    // cout_request(request);
}