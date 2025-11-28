#pragma once
#include <netinet/in.h>
#include <sys/epoll.h>

#define MAX_EVENTS 10

struct MclientsData
{
    int server_fd;
    struct sockaddr_in address;
    socklen_t addrlen = sizeof(address);
    struct epoll_event ev, events[MAX_EVENTS];
    int efd;
};
