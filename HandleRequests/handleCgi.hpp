#pragma once
#include "../includes/config.hpp"
#include "Executor.hpp"
#include "HttpRequest.hpp"
#include "HttpResponse.hpp"
#include <sys/types.h>
#include <sys/wait.h>
#include <sstream>

#define NAME_LEN 5

class Cgi {
private:

	HttpRequest		&request;
	HttpResponse	&response;
	Executor		&ex;
	std::string		http_Protocol;
	size_t			size;
	char			**envp;
	int				responseCode;

	void fill_char_array(char *c_str, std::string cppStr);
	void convertFromVectorStringtToDoubleArray(std::vector<std::string> &env);

	std::string pathResolver();

	void isValideFile(std::string &path);

	void createEnvp();

	std::pair<std::string, std::string> exrtactKeyValue(std::string line, size_t endOfValue);
	void shiftFileOffset(int fd, size_t len);

	void writeHeadersFromCgiOut(int fd, std::string filename);

	void resetFileOffset(int& fd, std::string filename);
	std::string generateRandomName();
	void createFile();

public:

	Cgi(HttpRequest &request, HttpResponse &resp, Executor &ex);
	~Cgi();

	void executeCgi(void);
	int getResponseCode();

};
