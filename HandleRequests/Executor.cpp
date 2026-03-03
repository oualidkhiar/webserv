#include "Executor.hpp"
#include "StringManip.hpp"
#include <sys/stat.h>
#include "enums.hpp"
#include <unistd.h>
#include "MimeTypes.hpp"
#include <algorithm>
#include <fstream>
#include <sstream>
#include <fcntl.h>
#include <dirent.h>

Executor::Executor(): Case(NONE) {}

ExecutorCase Executor::getExecutorCase() {return this->Case;}

std::string Executor::pathResolverForDelete(HttpRequest &request)
{
    std::string path;

    path = request.getLocation()->rootPath + request.getUri();
    return (path);
}


void Executor::executeDelete(HttpRequest &request, HttpResponse &response)
{
    std::string path = pathResolverForDelete(request);
    struct stat sb;
    if (stat(path.c_str(), &sb) != 0)
    {
        response.setStatus(HP_NOT_FOUND);
        return;
    }
    if (access(path.c_str(), W_OK) != 0)
    {
        response.setStatus(HP_FORBIDDEN);
        return;
    }
    if (unlink(path.c_str()) != 0)
    {
        response.setStatus(HP_INTERNAL_SERVER_ERROR);
        return;
    }
    // 0 is for success yak ?
    response.setStatus(0);

    // here we sure thats request executed successfully so we can set the response header and body
    response.AddHeader("Content-Length", "0\r\n");
    response.AddHeader("Connection", "closed\r\n");
    response.AddHeader("server", "TestServer/1.1\r\n");
    response.setState(RESPONSE_FINISHED);
}


std::pair<int, FtFile *> Executor::extractFileInfos(const char *path)
{
    FtFile *file;
    struct stat sb;
    if (stat(path, &sb) != 0)
        return (std::make_pair(HP_NOT_FOUND, (FtFile *)NULL));
    if (access(path, R_OK) != 0)
        return (std::make_pair(HP_FORBIDDEN, (FtFile *)NULL));
    file = new FtFile(path);
    file->setFileSize(sb.st_size);
    return (std::make_pair(1, file));
}

void Executor::setLocation(HttpRequest &request)
{
    location *bestLocation;
    bestLocation = getLongestMatchedLocation(request, request.getConfig()->Locations);
    std::cout << "best match is " << bestLocation->key << std::endl;
    std::cout << "uri = " << request.getUri() << std::endl;
    request.setLocation(bestLocation);
}

bool isDirectory(const std::string &path) {
    struct stat s;
    if (stat(path.c_str(), &s) != 0)
        return false;
    return S_ISDIR(s.st_mode);
}

std::pair<int, std::string> Executor::pathResolver(HttpRequest &request)
{
    std::string path;
    std::string uri = request.getUri();
    path = request.getLocation()->rootPath + uri;
    if (isDirectory(path)) {
        if (uri[uri.length()-1] != '/') {
            return std::make_pair(301, uri+"/"); // case redirection 
        }
        else if (request.getLocation()->redirection.second.length() > 0) { // case redirection from config file
            return std::make_pair(request.getLocation()->redirection.first,
                    request.getLocation()->redirection.second);
        }
        else if (request.getLocation()->indexFiles.size() > 0) { // case index file exist
            return std::make_pair(2, "");
        }
        else if (request.getLocation()->autoindex) { // open dir and generate a list of what that dir contain
            return std::make_pair(3, path);
        }
        else {
            return std::make_pair(4, ""); // forbiden
        }
    }
    return std::make_pair(5, path); // regular request
}

bool Executor::isAllowedMethod(HttpRequest &request)
{
    std::string methods[3] = {"DELETE", "POST", "GET"};
    std::set<std::string> *allowed_methods = &(request.getLocation()->allowMethods);
    if (allowed_methods->find(methods[request.getType()]) == allowed_methods->end())
        return (false);
    return (true);
}

void Executor::execute(HttpRequest &request, HttpResponse &response)
{
    CGIType cgiType = request.getCGIType();
	if (request.getType() == POST) {
        if (cgiType == PHP_CGI or cgiType == PYTHON_CGI or cgiType == SHELL_CGI) { // check if request is cgi
            this->Case = CGI_EXECUTION;
            return ;
        }
        response.setState(RESPONSE_FINISHED);
		return ;
    }
    if (cgiType == PHP_CGI or cgiType == PYTHON_CGI or cgiType == SHELL_CGI) { // check if request is cgi
        this->Case = CGI_EXECUTION;
        return ;
    }
    if (request.getType() == DELETE) {
        executeDelete(request, response);
    }
    if (request.getType() == GET) {
        executeGet(request, response);
    }
}

void Executor::setContentTpe(HttpResponse &response, const std::string &path)
{
    size_t pos = path.rfind('.');
    std::string content_type;
    if (pos != std::string::npos)
    {
        content_type = MimeTypes::getType(path.substr(pos + 1));
        response.AddHeader(CONTENT_TYPE_HEADER, content_type);
    }
    else
        response.AddHeader(CONTENT_TYPE_HEADER, DEFAULT_CONTENT_TYPE);
}

void Executor::caseRedirection(HttpResponse& response, std::string& path, int code)
{
    response.AddHeader("location", path+"\r\n");
    response.setStatus(code);
    response.setState(RESPONSE_FINISHED);
}

std::pair<int, FtFile *> Executor::getIndexFile(HttpRequest& request)
{
    std::string indexFile;
    int lastFile;
    for (int i = 0; i < request.getLocation()->indexFiles.size(); i++)
    {
        indexFile = (request.getLocation()->rootPath + "/" + request.getLocation()->indexFiles[i]);
        std::pair<int, FtFile *> p = extractFileInfos(indexFile.c_str());
        if (p.first == 1)
        {
            return p;
        }
        lastFile = p.first;
    }
    return std::make_pair(lastFile, (FtFile *)NULL);
}
std::string extructFileName(std::string fullPath)
{
    std::size_t pos = fullPath.find_last_of('/');
    if (pos != std::string::npos) {
        return fullPath.substr(pos);
    }
    return "";
}

void Executor::caseIndexFile(HttpResponse& resp, HttpRequest& req)
{
    std::pair<int, FtFile *> res = getIndexFile(req);
    req.checkCGI(res.second->getPath());
    CGIType cgiType = req.getCGIType();
    if (cgiType == PHP_CGI or cgiType == PYTHON_CGI or cgiType == SHELL_CGI) { // maybe index file is a cgi (needs to execute : index.py ...)
        res.second->ft_close();
        std::string fileName = extructFileName(res.second->getPath());
        if (fileName.length() == 0) {
            resp.setStatus(HP_NOT_FOUND);
            resp.setState(READING_LARGE_FILE);
            return ;
        }
        req.setUri(fileName);
        this->Case = CGI_EXECUTION;
        return ;
    }
    if (res.first != 1) {
        resp.setStatus(res.first);
        resp.setState(READING_LARGE_FILE);
        return ;
    }
    setContentTpe(resp, res.second->getPath());
    resp.setFile(res.second);
    resp.setState(READING_LARGE_FILE);
    resp.createBody();
}

std::vector<unsigned char> convertToVector(std::string body)
{
    std::vector<unsigned char> res;
    for (size_t i = 0; i < body.size(); i++) {
        res.push_back(body[i]);
    }
    return res;
}

std::string buildListInHtmlFormat(std::string& uri, DIR *dir)
{
    std::stringstream html;
    struct dirent *entry;

    html << "<html>\n";
    html << "<head><title>Index of " << uri << "</title></head>\n";
    html << "<body>\n";
    html << "<h1>Index of " << uri << "</h1>\n";
    html << "<hr>\n";
    html << "<ul>\n";
    while ((entry = readdir(dir)) != NULL) {
        std::string name = entry->d_name;
         if (name[0] == '.')
            continue;
        std::string link = uri;
        if (uri[uri.size()-1] != '/')
            link += "/";
        link += name;
        html << "<li><a href=\"" << link
            << "\">" << name << "</a></li>";
    }
    html << "</ul>\n";
    html << "<hr>\n";
    html << "</body>\n";
    html << "</html>\n";
    return html.str();
}

void Executor::caseListingFiles(HttpResponse& resp, HttpRequest& req, std::string& path)
{
    DIR *dir = opendir(path.c_str());
    std::string uri = req.getUri();
    resp.createBody();
    if (!dir) {
        resp.setStatus(HP_FORBIDDEN);
        resp.setState(READING_LARGE_FILE);
        return;
    }
    std::string list = buildListInHtmlFormat(uri, dir);
    closedir(dir);
    std::vector<unsigned char> chunk = convertToVector(list);
    std::ostringstream content_len;
    content_len << list.length();
    resp.AddHeader("content-length", content_len.str()+"\r\n");
    resp.AddHeader("Connection", "closed\r\n");
    resp.AddHeader("server", "TestServer/1.1\r\n");
    resp.AddHeader("Content-Type", "text/html\r\n");
    resp.appendBodyToResponse(chunk);
    resp.setState(RESPONSE_FINISHED);
}

void Executor::caseSpecifiedFile(HttpResponse& response, std::string& path)
{
    std::pair<int, FtFile *> p = extractFileInfos(path.c_str());
    if (p.first != 1) {
        response.setStatus(p.first);
    } else {
        response.setFile(p.second);
        setContentTpe(response, p.second->getPath());
    }
    response.createBody();
    response.setState(READING_LARGE_FILE);
}

void Executor::caseForbiden(HttpResponse& resp)
{
    resp.setStatus(HP_FORBIDDEN);
    resp.setState(RESPONSE_FINISHED);
}

void Executor::executeGet(HttpRequest &request, HttpResponse &response)
{
    std::pair<int, std::string> p = pathResolver(request);
    std::pair<int, FtFile *> pair;
    switch (p.first)
    {
        case 301: // redirection for 301
        {
            caseRedirection(response, p.second, HP_MOVED_PERMANENTLY);
            break;
        }

        case 302: // redirection for 302
        {
            caseRedirection(response, p.second, HP_FOUND);
            break;
        }

        case 2: // index file exist send index file
        {
            caseIndexFile(response, request);
            break;
        }
        
        case 3: // case listing files from dir
        {
            caseListingFiles(response, request, p.second);
            break ;
        }
        
        case 4: // forbiden --> it is a directory but there is no index file and autoindex is false
        {
            caseForbiden(response);
            break;
        }

        case 5: // regular request with specify the target file 
        {
            caseSpecifiedFile(response, p.second);
            break;
        }

        default:
            break;
    }
}

std::string normalizeUri(std::string uri)
{
    std::string normalizedUri;
    bool seen = false;
    for (int i = 0; i < uri.length(); i++) {
        if (uri[i] == '/' and !seen) {
            normalizedUri.push_back(uri[i]);
            seen = true;
        } else if (uri[i] != '/') {
            normalizedUri.push_back(uri[i]);
            seen = false;
        }
    }
    return normalizedUri;
}

int Executor::matchedScore(const std::string uri, const std::string key)
{
    if (uri.compare(0, key.length(), key) != 0) 
        return 0;
    if (uri.length() == key.length())
        return key.length();
    if (uri[key.length()] == '/')
        return key.length();
    return 0;
}

location* Executor::getLongestMatchedLocation(HttpRequest &request, 
                            std::map<std::string, location *> map)
{
    request.setUri(normalizeUri((request.getUri())));
    std::string uri = request.getUri();
    location *best_match = NULL;
    int best_score = 0;

    for (std::map<std::string, location *>::const_iterator it = map.begin(); it != map.end(); ++it)
    {
        int score = matchedScore(uri, it->first);
        if (score > best_score)
        {
            best_match = it->second;
            best_score = score;
        }
    }
    if (!best_match && map.count("/"))
        best_match = map["/"];

    return best_match;
}
