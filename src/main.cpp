#include "../includes/webserv.hpp"

class client {
private:
    int			fd;
    bool		RequestIsFull;
    bool		ResponseIsComplet;
    std::string	ReceiveBuffer;
    std::string	ResponseBuffer;
public:
	client() {
		RequestIsFull = false;
		ResponseIsComplet = false;
	}

};

class server {
private:
    int                 serverFd;
    int                 clientFd;
    struct sockaddr_in  address;
    socklen_t           addLen;
    std::unordered_map<int, client> clients;
    std::unordered_map<std::string, std::string> Config;
public:
    server() {
        address.sin_port = htons(PORT);
        address.sin_family = AF_INET;
        address.sin_addr.s_addr = INADDR_ANY;
        addLen = sizeof(address);
    }
    ~server() {
        close(serverFd);
    }
    int serverCreating() {
        serverFd = socket(AF_INET, SOCK_STREAM, 0);
        if (serverFd < 0) {
            perror("socket failed");
            return (-1);
        }
        return (0);
    }
    int serverBinding() {
        int opt = 1;
        setsockopt(serverFd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));
        if (bind(serverFd, (struct sockaddr *)(&address), addLen) == -1) {
            perror("bind failed");
            close(serverFd);
            return (-1);
        }
        return (0);
    }
    int serverListening() {
        if (listen(serverFd, 5) == -1) {
            perror("listen failed");
            close (serverFd);
            return (-1);
        }
        return (0);
    }
    void acceptIncommingConections() {
        while (true) {
            clientFd = accept(serverFd, (struct sockaddr *)(&address), &addLen);
            if (clientFd < 0) {
                perror("accept failed");
                continue ;
            }
            char buffer[1024];
            read(clientFd, buffer, 500);
            write(1, buffer, 500);
            
        }
    }
};

int main()
{
    server serv;
    
    if (serv.serverCreating() != -1 && \
        serv.serverBinding() != -1 && \
        serv.serverListening() != -1)
        serv.acceptIncommingConections();
    else
        return (1);
    return (0);
}



// multiplexing


// create a server listining in a specific port 
// parse request from client 
// build a response for this client 
// handle multiple client (multiplexing)
// parse the config file it is valid
// checking the request from client if it follow the rules from config file

// class Solution {
// public:
//     void rotate(vector<vector<int>>& matrix) {
//         for (int i = matrix.size() - 1; i > 0; i--)
//         {
            
//         }
//     }
// };

// int main()
// {
//     Solution sol;
//     vector<vector<int>> res = {
//                                 {1,2,3},
//                                 {4,5,6},
//                                 {7,8,9}
//                                         };

//     sol.rotate(res);

//     cout << "[\n";
//     for (int i = 0; i < res.size(); i++) 
//     {
//         cout << "[";
//         for (int j = 0; j < res[i].size(); j++)
//         {
//             cout << res[i][j] << ", ";
//         }
//         cout << "]\n";
//     }
//     cout << "          ]\n";
// }


