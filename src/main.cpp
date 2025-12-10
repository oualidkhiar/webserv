#include "../includes/server_manager.hpp"

int main(int ac, char **av)
{
    if (ac != 2) {
        std::cout << "invalid argument" << std::endl;
        return 2;
    }

    // ServerManager server(av[1]);

    // server.StartAllServers();
    // if (server.checkError())
    //     return 2;
    // server.TrackSocketsEvent();

    config conf(av[1]);

    return (0);
}
