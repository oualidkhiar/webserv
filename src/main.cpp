#include "../includes/server_manager.hpp"
#include "../HandleRequests/TransactionManager.hpp"
// #include "../HandleRequests/RequestParser.hpp" // debugging

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


/* DEBUGGING 

void PrintVector(std::vector<unsigned char> v)
{
	// std::cout << v.empty();
	for (size_t i = 0; i < v.size(); i++)
	{
		if (v[i] == '\n')
		{
			std::cout << "\\n" << std::endl;
		}
		else if (v[i] == '\r')
			std::cout << "\\r";
		else
    		std::cout << v[i];
	}
	std::cout << std::endl;
}

int main(int ac, char **av)
{
	RequestParser parser;
	HttpRequest request;
	std::string R;	
	R = "7\r\n"
		"Mozilla\r\n"
		"9\r\n"
		"Developer\r\n"
		"7\r\n"
		"Network\r\n"
		"0\r\n"
		"\r\n";
	std::vector<unsigned char> v(R.begin(), R.end());
	request.setRequest(v);
	parser.read_body_chunked(request);
	
	std::cout << "Printing body: " << std::endl;
	PrintVector(request.getBody().getBody());
	std::cout << std::endl;
	std::cout << "Printing request: " << std::endl;

	PrintVector(request.getRequest());
}
*/