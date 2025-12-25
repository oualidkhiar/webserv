#include <iostream>
#include <string>
#include <cstring>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>

int main()
{
    int sock = 0;
    struct sockaddr_in serv_addr;
    char buffer[4096] = {0};

    // 1. Create the socket
    if ((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0)
    {
        std::cerr << "Socket creation error" << std::endl;
        return -1;
    }

    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(80); // Nginx default port

    // Convert IPv4 address from text to binary (connecting to localhost)
    if (inet_pton(AF_INET, "127.0.0.1", &serv_addr.sin_addr) <= 0)
    {
        std::cerr << "Invalid address/ Address not supported" << std::endl;
        return -1;
    }

    // 2. Connect to the local Nginx server
    if (connect(sock, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0)
    {
        std::cerr << "Connection Failed. Is Nginx running?" << std::endl;
        return -1;
    }

    // 3. Manually typed HTTP Request String
    // IMPORTANT: Each line must end in \r\n, and the request must end with a blank line (\r\n\r\n)
    std::string httpRequest =
        "GET / HTTP/1.1 \r\n"
        "Host: site2.com\r\n"
        "Connectionclose\r\n"
        "User-Agent: ManualClient/1.0\r\n"
        "\r\n";

    // 4. Send the string
    send(sock, httpRequest.c_str(), httpRequest.length(), 0);
    std::cout << "Request sent...\n"
              << std::endl;

    // 5. Read the response
    int valread = read(sock, buffer, 4096);
    if (valread > 0)
    {
        std::cout << "--- Nginx Response ---\n"
                  << std::endl;
        std::cout << buffer << std::endl;
    }

    // 6. Close the socket
    close(sock);

    return 0;
}
