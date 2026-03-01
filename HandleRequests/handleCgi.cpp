#include "handleCgi.hpp"
#include "utils.hpp"
#include <unistd.h>
#include <sys/stat.h>
#include <ctime>
#include "Executor.hpp"

Cgi::Cgi(HttpRequest &request, HttpResponse &resp) : request(request), 
response(resp), responseCode(0), size(0), pid(-1)
{
	http_Protocol = "HTTP/1.1";
}

Cgi::~Cgi()
{
	if (size > 0) {
		for (size_t i = 0; i < size; i++)
		{
			delete[] envp[i];
		}
		delete[] envp;
	}
}

void Cgi::fill_char_array(char *c_str, std::string cppStr)
{
	for (int i = 0; i < cppStr.length(); i++)
	{
		c_str[i] = cppStr[i];
	}
	c_str[cppStr.length()] = '\0';
}

void Cgi::convertFromVectorStringtToDoubleArray(std::vector<std::string> &env)
{
	envp = new char *[env.size() + 1];

	for (int i = 0; i < env.size(); i++)
	{
		envp[i] = new char[env[i].length() + 1];
		fill_char_array(envp[i], env[i]);
	}
	envp[env.size()] = NULL;
	size = env.size();
}

std::string Cgi::pathResolver()
{
	std::string path;
	if (request.getLocation() != NULL)
	{
		if (request.getLocation()->rootPath.empty() == false)
			path = request.getLocation()->rootPath + request.getUri();
	}
	return (path);
}

void Cgi::isValideFile(std::string &path)
{
	if (access(path.c_str(), F_OK) != 0)
	{
		this->responseCode = 404;
			// path not found error response generate 404
		return ;
	}
	if (access(path.c_str(), X_OK) != 0)
	{
		this->responseCode = 403;
		return ; // file not executeble error response permession denid
	}
}

void Cgi::createEnvp()
{
	std::vector<std::string> env;
	std::string value;

	value = request.getType();
	value = "REQUEST_METHOD=" + value;
	env.push_back(value);

	if (request.getQuery().length() > 0)
	{
		value = "QUERY_STRING=" + request.getQuery();
		env.push_back(value);
	}

	value = "SCRIPT_NAME=" + request.getUri();
	env.push_back(value);

	value = "SERVER_PROTOCOL=" + http_Protocol;
	env.push_back(value);

	value = request.getHeader("Content-Type");
	if (value.length() > 0)
	{
		value = "CONTENT_TYPE=" + value;
		env.push_back(value);
	}

	value = request.getHeader("Content-Length");
	if (value.length() > 0)
	{
		value = "CONTENT_LENGTH=" + value;
		env.push_back(value);
	}

	value = request.getHeader("Host");
	if (value.length() > 0)
	{
		value = "HTTP_HOST=" + value;
		env.push_back(value);
	}

	value = request.getHeader("User-Agent");
	if (value.length() > 0)
	{
		value = "HTTP_USER_AGENT=" + value;
		env.push_back(value);
	}
	convertFromVectorStringtToDoubleArray(env);
}

std::pair<std::string, std::string> Cgi::exrtactKeyValue(std::string line, size_t endOfValue)
{
	std::pair<std::string, std::string> ret;
	size_t pos;

	pos = line.find(':');
	if (pos == std::string::npos)
	{ //':' not found should treat error 500
		this->responseCode = 500;
		return (ret);
	}
	ret.first = line.substr(0, pos);
	pos += 2; // skipe space and ':'
	ret.second = line.substr(pos, endOfValue - pos);
	return (ret);
}

void Cgi::shiftFileOffset(size_t len)
{
	std::cout << len << std::endl;
	char *buffer = new char[len];
	if (!buffer)
	{
		this->responseCode = 500;
		delete[] buffer;
		return ;
	}
	int bytes_read = read(response.getFile()->getFd(), buffer, len);
	if (bytes_read < 0)
	{
		this->responseCode = 500;
	}
	delete[] buffer;
}

void Cgi::writeHeadersFromCgiOut( void )
{
	std::ifstream file(response.getFile()->getPath().c_str());
	if (!file.is_open())
	{
		this->responseCode = 500;
		return ;
	}
	std::string header;
	size_t offsetShiftedCounter = 0;
	std::pair<std::string, std::string> keyVal;
	size_t pos;

	getline(file, header);
	pos = header.length() - 1;
	if (header[pos] == '\r')
	{
		keyVal = exrtactKeyValue(header, pos);
		if (this->responseCode != 0)
		{
			file.close();
			return ;
		}
		keyVal.second += "\r\n";
		response.overWriteHeader(keyVal);
		offsetShiftedCounter += header.length() + 1;
	}
	else
	{
		file.close();
		return ;
	}
	while (getline(file, header))
	{
		pos = header.length() - 1;
		if (header == "\r")
		{
			offsetShiftedCounter += 2;
			break ;
		}
		if (header[pos] == '\r')
		{
			keyVal = exrtactKeyValue(header, pos);
			if (this->responseCode != 0)
			{
				break ;
			}
			keyVal.second += "\r\n";
			response.overWriteHeader(keyVal);
			offsetShiftedCounter += header.length() + 1;
		}
		else
		{
			this->responseCode = 500;
			break ;
		}
	}
	file.close();
	response.getFile()->setFileSize(sb.st_size - offsetShiftedCounter);
	shiftFileOffset(offsetShiftedCounter);
}

void Cgi::resetFileOffset()
{
	int		fd;

	close(response.getFile()->getFd());
	fd = open(response.getFile()->getPath().c_str(), O_RDONLY);
    if (fd < 0) {
        perror("open");
        this->responseCode = 500;
    }
	response.getFile()->setFd(fd);
	response.getFile()->setState(FILE_READING);
	response.getFile()->setRemoveFile(true);
    if (stat(response.getFile()->getPath().c_str(), &sb) != 0) {
		this->responseCode = 500;
		return ;
	}
    response.setState(READING_LARGE_FILE);
}

void Cgi::createResponse()
{
	std::string filename;
	std::string outfile;
	FtFile 		*file;
	int			fd;

	filename = generateRandomName(); // generate random
	outfile = "/tmp/"+filename;
	fd = open(outfile.c_str(), O_CREAT | O_WRONLY, 0644);
	if (fd < 0) {
		perror("open");
		this->responseCode = 500;
		return ;
	} else {
		file = new FtFile(outfile);
		file->setFd(fd);
		response.setFile(file);
		response.createBody();
	}
}

void Cgi::redirectOutOnly()
{
	dup2(response.getFile()->getFd(), STDOUT_FILENO);
	close(response.getFile()->getFd());
}

void Cgi::redirectInOut()
{
	// dup2(request.outfile()->getFd(), STDIN_FILENO);
	dup2(response.getFile()->getFd(), STDOUT_FILENO);
	close(response.getFile()->getFd());
	// close(infile)
}

bool Cgi::timeOut()
{
	static time_t 	startTime;
	time_t			currentTime;

	if (startTime == 0) {
		startTime = time(NULL);
	}
	currentTime = time(NULL) - startTime;
	return currentTime >= MAX_TIME_RUN;
}

void Cgi::parentPs()
{
	int		status;
	pid_t	result;
	result = waitpid(pid, &status, WNOHANG);
	if (result != 0) {
		if (status != 0 || result == -1) {
			this->responseCode = 500;
			return ;
		}
		resetFileOffset();
    	if (this->responseCode != 0) {return ;}
		writeHeadersFromCgiOut();
		if (response.getHeader(CONTENT_TYPE_HEADER) == "NOT_FOUND") {
			response.AddHeader(CONTENT_TYPE_HEADER, DEFAULT_CONTENT_TYPE);
		}
		if (request.getType() == POST) {
			// close(infile);
			// remove it 
		}
		return ;
	}
	if (this->timeOut()) {
		kill(pid, SIGKILL);
		this->responseCode = 504; // Gateway timeout error response 
	}
}

void Cgi::childPs(std::string path)
{
	char *argv[2];
	argv[0] = (char *)path.c_str();
	argv[1] = NULL;
	if (request.getType() == GET || request.getType() == DELETE) {
		redirectOutOnly();
	} else {
		redirectInOut();
	}
	// TODO: if python get interpreter for python / shell interpreter for shell / php intepreter for php --> from config file 
	std::cerr << path.c_str() << std::endl;
	execve(path.c_str(), argv, envp);
	exit(1);
}

void Cgi::executeCgi(void)
{
	std::string path = pathResolver();
	isValideFile(path);
	if (this->responseCode != 0) {
		return ;
	}
	createResponse();
	if (this->responseCode != 0) {
		std::cout << path << std::endl;
		return ;
	}
	createEnvp();
	pid = fork();
	if (pid < 0) {
		perror("fork");
		this->responseCode = 500;
	}
	else if (pid == 0) {
		childPs(path);
	}
	else {
		this->response.setState(WAITING_FOR_CGI);
		usleep(1000); // sleep parent 1000 microsecond maybe child will finish fast
		parentPs();
	}
}


void Cgi::isChildFinishExecute_Cgi()
{
	this->parentPs();
	if (this->responseCode != 0)
	{
		this->response.setStatus(this->responseCode);
		this->response.setState(RESPONSE_FINISHED);
	}
}

int Cgi::getResponseCode() {return (this->responseCode);}