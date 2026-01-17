#include "Executor.hpp"
#include "StringManip.hpp"
#include <sys/stat.h>
#include "enums.hpp"
#include <unistd.h>
#include "MimeTypes.hpp"
#include <algorithm>
#include "handleCgi.hpp"
Executor::Executor() {}

void Executor::executeDelete(HttpRequest &request, HttpResponse &response)
{
    (void)request;
    {
        // definition here
    }
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
    request.setLocation(bestLocation);
}

std::string Executor::pathResolver(HttpRequest &request)
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
    setLocation(request);
    if (!isAllowedMethod(request))
    {
        response.setStatus(HP_FORBIDDEN);
        return;
    }
    if (request.getType() == DELETE)
        executeDelete(request, response);
    else if (request.getType() == GET) {
        // executeGet(request, response);
        Cgi c(request, response, *this);
        c.executeCgi();
        // if (c.getResponseCode() != 0) {
        //     // response with spicifique error 
        // }
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

void Executor::executeGet(HttpRequest &request, HttpResponse &response)
{
    std::vector<unsigned char> file_content;  // not used ??
    std::string path = pathResolver(request);
    std::pair<int, FtFile *> pair;
    pair = extractFileInfos(path.c_str());
    if (pair.first != 1)
    {
        response.setStatus(pair.first);
        exit_error("Stat Eroor");
    }
    setContentTpe(response, path);
    response.setFile(pair.second);
    if (response.getFile()->getFileSize() > MAX_FILE_READ)
        response.setState(READING_LARGE_FILE);
    response.createBody();
}

int Executor::matchedScore(std::string uri, std::string key)
{

    int score = 1;
    int i = 0;
    std::vector<std::string> key_tokens;
    std::vector<std::string> uri_tokens;
    if ((tokensSize(key, PATH_DELIMITER) < tokensSize(uri, PATH_DELIMITER)))
    {
        key_tokens = ft_split(key, PATH_DELIMITER);
        uri_tokens = ft_split(uri, PATH_DELIMITER);
        int size = key_tokens.size();
        while (i < size)
        {
            if (key_tokens.at(i).compare(uri_tokens.at(i)) != 0)
                return (0);
            i++;
        }
    }
    return (i);
}
location *Executor::getLongestMatchedLocation(HttpRequest &request, std::map<std::string, location *> map)
{
    std::string uri = request.getUri();
    //  int best_expected_score = tokensSize(uri, PATH_DELIMITER);
    // std::cout<<"best_expected_score  = "<<best_expected_score<<std::endl;

    location *best_match = NULL;
    int previous_score = 0;
    int score = -1;
    for (std::map<std::string, location *>::const_iterator it = map.begin(); it != map.end(); ++it)
    {
        score = matchedScore(uri, it->first);
        if (score > previous_score)
        {
            best_match = it->second;
            previous_score = score;
        }
    }

    return (best_match);
}
