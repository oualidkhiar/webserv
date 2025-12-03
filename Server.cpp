#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#include "MclientsData.hpp"
#include <vector>
#include <string>
#include <iostream>

#define PORT 8080
#define BUFFER_SIZE 1024

std ::vector<std::string> split(std ::string &s, std ::string delimiter)
{
    int pos;
    std ::vector<std ::string> tokens;
    while ((pos = s.find(delimiter)) != std::string::npos)
    {
        std::string token = s.substr(0, pos);
        if (token.size() > 0)
            tokens.push_back(token);
        s.erase(0, pos + delimiter.length());
    }
    return (tokens);
}

int send_foward_message(struct MclientsData *data, int client_socket_fd)
{
    std ::string remainder;
        char message_server[BUFFER_SIZE] = "MESSAGE FROM SERVER ";
        char buffer[BUFFER_SIZE];
        int readsize = recv(client_socket_fd, &buffer, BUFFER_SIZE, 0);
        std :: cout << "mesage RESIVED" <<std::endl <<"MESSAGE : " << buffer << std::endl; 
        send(client_socket_fd ,message_server ,  strlen(message_server) , 0);
    return (1);
}
int accept_new_connection(struct MclientsData *data)
{
    int new_socket;
    while (1)
    {
        new_socket = accept(data->server_fd, (struct sockaddr *)&data->address, &data->addrlen);
        if (new_socket == -1)
        {
            if (errno == EAGAIN || errno == EWOULDBLOCK)
                break;
            else
            {
                perror("ACCEPT FAILED ");
                exit(EXIT_FAILURE);
            }
        }
        fcntl(new_socket, F_SETFL, O_NONBLOCK);
        data->ev.events = EPOLLIN | EPOLLOUT | EPOLLET;
        data->ev.data.fd = new_socket;
        if (epoll_ctl(data->efd, EPOLL_CTL_ADD, new_socket, &(data->ev)) == -1)
        {
            perror("EPOLL_CTL : ");
            exit(EXIT_FAILURE);
        }
    }
    return (1);
}
int main(void)
{
    int new_socket;
    ssize_t valread;
    struct MclientsData data;

    int opt = 1;
    data.addrlen = sizeof(data.address);
    char buffer[1024] = {0};
    char hello[20] = "Hello from server";

    if ((data.server_fd = socket(AF_INET, SOCK_STREAM, 0)) < 0)
    {
        perror("socket failed");
        exit(EXIT_FAILURE);
    }

    if (setsockopt(data.server_fd, SOL_SOCKET,
                   SO_REUSEADDR | SO_REUSEPORT, &opt,
                   sizeof(opt)))
    {
        perror("setsockopt");
        exit(EXIT_FAILURE);
    }
    data.address.sin_family = AF_INET;
    data.address.sin_addr.s_addr = INADDR_ANY;
    data.address.sin_port = htons(PORT);

    if (bind(data.server_fd, (struct sockaddr *)&data.address,
             sizeof(data.address)) < 0)
    {
        perror("bind failed");
        exit(EXIT_FAILURE);
    }
    if (listen(data.server_fd, 3) < 0)
    {
        perror("listen");
        exit(EXIT_FAILURE);
    }
    fcntl(data.server_fd, F_SETFL, O_NONBLOCK);
    //----------------------------------------------------------------------------------------------------//

    data.efd = epoll_create1(0);

    if (data.efd == -1)
    {
        perror("epoll_create");
        exit(EXIT_FAILURE);
    }

    data.ev.data.fd = data.server_fd;

    data.ev.events = EPOLLIN | EPOLLOUT | EPOLLET;

    if (epoll_ctl(data.efd, EPOLL_CTL_ADD, data.server_fd, &data.ev) == -1)
    {
        perror("epoll_ctl");
        exit(EXIT_FAILURE);
    }
    //----------------------------------------------------------------------------------------------------//

    //--------------------------------- INFINITE LOOP ACCEPt CONNECTION --------------------------------------//
    while (1)
    {
        int nfds = epoll_wait(data.efd, data.events, MAX_EVENTS, -1);
        if (nfds == -1)
        {
            perror("epoll_wait Failed : ");
            exit(EXIT_FAILURE);
        }
        int i = 0;`
        while (i < nfds)
        {
            int fd = data.events[i].data.fd;
            if (fd == data.server_fd)
            {
                accept_new_connection(&data);
            }
            else if ((data.events[i].events & EPOLLERR) ||
                     (data.events[i].events & EPOLLHUP) ||
                     (!(data.events[i].events & EPOLLIN)))
            {
                close(fd);
            }
            else
            {
                send_foward_message(&data, fd);
            }
        }
    }
    //----------------------------------------------------------------------------------------------------//

    close(new_socket);

    close(data.efd);
    close(data.server_fd);
    return 0;
}