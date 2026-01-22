#include "../includes/server_manager.hpp"
#include "../HandleRequests/TransactionManager.hpp"

size_t ft_strlen(unsigned char *s)
{
    int i = 0;
    while (s[i])
        i++;
    return (i);
}

int main(int ac, char **av)
{
    if (ac != 2)
    {
        std::cout << "invalid argument" << std::endl;
        return 2;
    }

    config conf(av[1]);
    TransactionManager manager;
    conf.buildServersConfig();
    if (conf.CheckParse())
        return (2);
    serverConfig *server = conf.getSerevrConfig(0);
    manager.setServer(server);
    unsigned char simpleGet[500] =
        "GET /cgi-bin/test.cgi HTTP/1.1\r\n"
        "Host: localhost:8080\r\n"
        "Accept: application/json\r\n"
        "User-Agent: TestingClient/1.0\r\n"
        "Connection: close\r\n"
        "\r\n"; // End of Headers (No body follows)
    manager.appendToRequest(simpleGet, ft_strlen(simpleGet));
    while (manager.getResponseState() != RESPONSE_FINISHED)
    {
        std::pair<unsigned char * , size_t> reponse  = manager.getResponse();
        write(1, reponse.first , reponse.second);
        delete [] reponse.first;
    }
}
