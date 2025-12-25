#include "Executor.hpp"

Executor::Executor(std::vector<serverConfig *> *servers) { this->servers = servers; }

void Executor::setServer(HttpRequest &request)
{
    std::string server_name = request.getHeader("HOST");
    int server_names;
    if (server_name.empty() == true)
        request.setConfig(servers->at(0));
    for (std::vector<serverConfig *>::iterator it = servers->begin(); it != servers->end(); ++i)
    {
        if ((*it)->Port == request.getPort())
            request.setConfig(*it);
    }
}

HttpResponse Executor::execute(HttpRequest &request)
{
    setServer(request);
    if (request.getType() == DELETE)
        return (executeDelete(request));
    else if (request.getType() == GET)
        return (executeGet(request));
}

HttpResponse Executor::executeGet(HttpRequest &request)
{
    // To Implement now
}

location *Executor::getLongestMatchedLocation(std::string &fullpath, std::map<std::string, location *> map)
{
    location *location;

    return (location);
}