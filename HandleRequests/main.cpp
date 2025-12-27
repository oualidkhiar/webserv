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
        std::cout << "---BODY-----" << std::endl;
        for (std::vector<char>::const_iterator it = request.getBody()->getBody().begin(); it < request.getBody()->getBody().end(); ++it)
        {
            std::cout << *it;
        }
        std::cout << std::endl;
    }
}

int main()
{
#include <string>

    char *chunkedPost =
        "POST /api/users/create HTTP/1.1\r\n"
        "Host: localhost:8080\r\n"
        "Content-Type: application/json\r\n"
        "Transfer-Encoding: chunked\r\n"
        "User-Agent: TestingClient/1.0\r\n"
        "\r\n"   // End of Headers (Double CRLF)
        "17\r\n" // Chunk 1 size (23 in decimal)
        "{\"username\": \"jdoe\",\r\n"
        "2D\r\n" // Chunk 2 size (45 in decimal)
        "\"email\": \"jane.doe@example.com\", \"role\": \"admin\"}\r\n"
        "0\r\n" // Final Chunk (End of data)
        "\r\n"; // Final CRLF to terminate the message
    HttpRequest request;
    request.appendRequestData(chunkedPost, sizeof(chunkedPost));
    RequestParser request_parser;
    while (request.getStatus() != FINISHED)
        request_parser.create_request(request);
    cout_request(request);
}