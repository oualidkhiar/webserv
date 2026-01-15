#pragma once
#include "HttpRequest.hpp"
#include "HttpResponse.hpp"
#include "../includes/config.hpp"
#include "Executor.hpp"
#include <sys/types.h>
#include <sys/wait.h>

class Cgi {
private:

    HttpRequest& request;
    std::string http_Protocol;
    char        **envp;
    size_t      size;

public:
    Cgi(HttpRequest& request): request(request) {}
    ~Cgi() {
        // for (size_t i = 0; i < size; i++) {
        //     delete envp[i];
        // }
        // delete[] envp;
    }

    void fill_char_array(char *c_str, std::string cppStr)
    {
        for (int i = 0; i < cppStr.length(); i++) 
        {
            c_str[i] = cppStr[i];
        }
        c_str[cppStr.length()] = '\0';
    }

    void convertFromVectorStringtToDoubleArray(std::vector<std::string>& env)
    {
        envp = new char *[env.size() + 1];

        for (int i = 0; i < env.size(); i++) {
            envp[i] = new char[env[i].length() + 1];
            fill_char_array(envp[i] ,env[i]);
        }
        envp[env.size()] = NULL;
        size = env.size();
    }

    std::string pathResolver()
    {
        std::string path;
        if (request.getLocation() != NULL)
        {
            if (request.getLocation()->rootPath.empty() == false)
                path = request.getLocation()->rootPath + request.getUri();
            else
                path = request.getConfig()->rootPath + request.getUri();
        }
        return (path);
    }

    int isValideFile(std::string& path)
    {
        if (access(path.c_str(), F_OK) != 0) {
            return 404; // path not found error response generate 404
        }
        if (access(path.c_str(), X_OK) != 0) {
            return 403; // file not executeble error response permession denid
        }
        return 0;
    }

    void createEnvp()
    {
        std::vector<std::string> env;
        std::string value;

        value = request.getType();
        value = "REQUEST_METHOD="+value;
        env.push_back(value);
    
        if (request.getQuery().length() > 0) {
            value = "QUERY_STRING="+request.getQuery();
            env.push_back(value);
        }

        value = "SCRIPT_NAME="+request.getUri();
        env.push_back(value);

        value = "SERVER_PROTOCOL="+http_Protocol;
        env.push_back(value);

        value = request.getHeader("Content-Type");
        if (value.length() > 0) {
            value = "CONTENT_TYPE="+value;
            env.push_back(value);
        }

        value = request.getHeader("Content-Length");
        if (value.length() > 0) {
            value = "CONTENT_LENGTH="+value;
            env.push_back(value);
        }

        value = request.getHeader("Host");
        if (value.length() > 0) {
            value = "HTTP_HOST="+value;
            env.push_back(value);
        }

        value = request.getHeader("User-Agent");
        if (value.length() > 0) {
            value = "HTTP_USER_AGENT="+value;
            env.push_back(value);
        }
        convertFromVectorStringtToDoubleArray(env);
    }

    int executeCgi( void )
    {
        std::string path = pathResolver();
        // path = "./www/cgi-bin/hello.cgi";
        std::cout << "hnaaa" << path << std::endl;
        int n = isValideFile(path);
        if (n != 0) {
            return n;
        }
        // n = matchConfigFileRules();
        // if (n != 0) {
        //     return n;
        // }
        createEnvp();
        pid_t pid;
        int fd = open("/home/oukhiar/goinfre/webserve/www/cgi_output/writehere.txt", O_RDWR | O_CREAT);
        if (fd < 0) {perror("open"), exit(1);}
        pid = fork();
        if (pid < 0) {perror("fork");exit(1);}
        else if (pid == 0)
        {
            char *argv[2];
            argv[0] = (char *)path.c_str();
            argv[1] = NULL;
            dup2(fd, STDOUT_FILENO);
            execve(path.c_str(), argv, envp);
        }
        else {
            waitpid(pid, NULL, 0);
            std::remove("/home/oukhiar/goinfre/webserve/www/cgi_output/writehere.html");
            exit(0);
        }
        // redirect and call call execve
        // call handle get;
        return 0; // success
    }
};



            //   cgi request example


//              POST /cgi-bin/login.cgi?debug=true HTTP/1.1     |
//              Host: example.com:8080                          |   
//              User-Agent: curl/8.4.0                          |
//              Accept: */*                                     |
//              Content-Type: application/x-www-form-urlencoded |
//              Content-Length: 27                              |
//              Cookie: session=abc123                          |
//                                                              |                                                               
//              username=walid&pwd=42                           |                                                               
//                                                              |


                // REQUEST_METHOD=POST
                // SCRIPT_NAME=/cgi-bin/test.cgi
                // QUERY_STRING=x=42
                // SERVER_PROTOCOL=HTTP/1.1
                // CONTENT_TYPE=application/json
                // CONTENT_LENGTH=15
                // HTTP_HOST=localhost:8080
                // HTTP_USER_AGENT=curl
