#include "server_manager.hpp"
#include "TransactionManager.hpp"

size_t ft_strlen(unsigned char *s)
{
    int i = 0;
    while (s[i])
        i++;
    return (i);
}

int main(int ac, char **av)
{
    if (ac != 2) {
        std::cout << "invalid argument" << std::endl;
        return 2;
    }

    config conf(av[1]);

    conf.buildServersConfig();
    if (conf.CheckParse())
        return (2);
    // conf.printServer();
    ServerManager server(conf);
    server.StartAllServers();
    if (server.checkError())
        return 2;
    server.TrackSocketsEvent();
    return (0);
}