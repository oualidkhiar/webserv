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
	std::string		http_Protocol;
	size_t			size;
	char			**envp;
	int				responseCode;
	struct stat		sb;

	void fill_char_array(char *c_str, std::string cppStr);
	void convertFromVectorStringtToDoubleArray(std::vector<std::string> &env);

	std::string pathResolver();

	void isValideFile(std::string &path);

	void createEnvp();

	std::pair<std::string, std::string> exrtactKeyValue(std::string line, size_t endOfValue);
	void shiftFileOffset(size_t len);

	void writeHeadersFromCgiOut();

	void resetFileOffset();
	std::string generateRandomName();
	void createResponse();

	void redirectOutOnly();
	void redirectInOut();
	void parentPs(pid_t pid);
	void childPs(pid_t pid, std::string path);



public:

	Cgi(HttpRequest &request, HttpResponse &resp);
	~Cgi();

	void executeCgi(void);
	int getResponseCode();

};
