#include "../includes/server_manager.hpp"

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
    server.TrackSocketsEvent();
    return (0);
}
