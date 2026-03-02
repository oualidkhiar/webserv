#pragma once
#include "../includes/config.hpp"
#include "HttpRequest.hpp"
#include "HttpResponse.hpp"
#include <sys/types.h>
#include <sys/wait.h>
#include <sstream>

#define MAX_TIME_RUN 5; // 5 SECOND 

class Cgi {
private:

	HttpRequest		&request;
	HttpResponse	&response;
	std::string		http_Protocol;
	size_t			size;
	char			**envp;
	int				responseCode;
	struct stat		sb;
	pid_t			pid;

	void fill_char_array(char *c_str, std::string cppStr);
	void convertFromVectorStringtToDoubleArray(std::vector<std::string> &env);

	std::string pathResolver();

	void isValideFileCgiPass(std::string &path);

	void createEnvp();

	std::pair<std::string, std::string> exrtactKeyValue(std::string line, size_t endOfValue);
	void shiftFileOffset(size_t len);

	void writeHeadersFromCgiOut();

	void resetFileOffset();
	void createResponse();

	void redirectOutOnly();
	void redirectInOut();
	void parentPs();
	void childPs(std::string path);
	bool timeOut();

public:

	Cgi(HttpRequest &request, HttpResponse &resp);
	~Cgi();

	void executeCgi(void);
	void isChildFinishExecute_Cgi();
	int getResponseCode();

};



