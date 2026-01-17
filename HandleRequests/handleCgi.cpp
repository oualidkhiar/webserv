#include "handleCgi.hpp"

Cgi::Cgi(HttpRequest &request, HttpResponse &resp, 
    Executor &ex) : request(request), response(resp), 
    ex(ex), responseCode(0), size(0)
{
	http_Protocol = "HTTP/1.1";
}

Cgi::~Cgi()
{
	for (size_t i = 0; i < size; i++)
	{
		delete envp[i];
	}
	delete[] envp;
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
		else
			path = request.getConfig()->rootPath + request.getUri();
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

void Cgi::shiftFileOffset(int fd, size_t len)
{
	char *buffer = new char[len];
	if (!buffer)
	{
		this->responseCode = 500;
		return ;
	}
	int bytes_read = read(fd, buffer, len);
	if (bytes_read < 0)
	{
		this->responseCode = 500;
	}
}

void Cgi::writeHeadersFromCgiOut(int fd, std::string filename)
{
	std::ifstream file(filename.c_str());
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
	shiftFileOffset(fd, offsetShiftedCounter);
}

void Cgi::resetFileOffset(int& fd, std::string filename)
{
	close(fd);
	fd = open(filename.c_str(), O_RDONLY);
    if (fd < 0) {
        perror("open");
        this->responseCode = 500;
    }
}

std::string Cgi::generateRandomName()
{
	int fd;
	char buffer[NAME_LEN+1];
	std::string name;
	fd = open("/dev/random", O_RDONLY);

	while (fd > 0)
	{
		int bytes_read = read(fd, buffer, NAME_LEN);
		if (bytes_read > 0) {
			int p = 0;
			while (p < bytes_read) {
				if (std::isprint(buffer[p])) {
					name.push_back(buffer[p]);
					if (name.length() >= NAME_LEN) {break;}
				}
				p++;
			}
		}
		else {break ;}
		if (name.length() >= NAME_LEN) {break;}
	}
	if (name.empty()) {
		std::ostringstream oss;
		oss << (&fd);
		name = oss.str();
	}
	if (fd > 0) {
		close(fd);
	}
	return name;
}

void Cgi::executeCgi(void)
{
	std::string path = pathResolver();
	isValideFile(path);
	if (this->responseCode != 0)
	{
		return ;
	}
	pid_t pid;
	std::string filename;
	std::string outfile;

	// n = matchConfigFileRules();
	// if (n != 0) {
	//     return (n);
	// }
	std::cout << "herererer\n\n" << std::endl;
	filename = generateRandomName(); // generate random
	outfile = "./www/cgi_output/" + filename;

	createEnvp();
	int fd = open(outfile.c_str(), O_CREAT | O_RDWR | O_TRUNC, 0644);
	if (fd < 0)
	{
		perror("open");
        this->responseCode = 500;
        return ;
	}
	pid = fork();
	if (pid < 0)
	{
		perror("fork");
		this->responseCode = 500;
	}
	else if (pid == 0)
	{
		char *argv[2];
		argv[0] = (char *)path.c_str();
		argv[1] = NULL;
		dup2(fd, STDOUT_FILENO);
		execve(path.c_str(), argv, envp);
	}
	else
	{
		waitpid(pid, NULL, 0);
		// request.setUri("/cgi_output/" + filename);
		ex.executeGet(request, response);
		resetFileOffset(fd, outfile);
        if (this->responseCode != 0) {return ;}
		response.getFile()->setFd(fd);
		response.getFile()->setState(FILE_READING);
		response.getFile()->setRemoveFile(true);
		writeHeadersFromCgiOut(fd, outfile);
	}
}

int Cgi::getResponseCode()
{
	return (this->responseCode);
}