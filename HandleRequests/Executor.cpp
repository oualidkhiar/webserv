#include "Executor.hpp"
#include "StringManip.hpp"
#include <sys/stat.h>
#include "enums.hpp"
#include <unistd.h>
#include "MimeTypes.hpp"

Executor::Executor() {      }

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
    return (std::make_pair(1 , file));
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

HttpResponse Executor::execute(HttpRequest &request)
{
    HttpResponse response;
    setLocation(request);
    if (!isAllowedMethod(request))
        return (response);
    if (request.getType() == DELETE)
        return (executeDelete(request));
    else if (request.getType() == GET)
        return (executeGet(request));
    return (response);
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


HttpResponse Executor::executeGet(HttpRequest &request)
{
    std::vector<unsigned char> file_content;
    HttpResponse response;
    std::string path = pathResolver(request);
    std::pair <int ,FtFile *> pair;
    pair = extractFileInfos(path.c_str());
    if (pair.first != -1)
    {

        response.setStatus(pair.first);
        return (response);
    }
    setContentTpe(response, path);
    response.setFile(pair.second);
    file_content = response.getFile()->readFile();
    response.appendBodyToResponse(file_content);
    return (response);
}

int Executor::matchedScore(std::string uri, std::string key)
{
    int score = 0;
    std::vector<std::string> key_tokens;
    std::vector<std::string> uri_tokens;
    if ((tokensSize(key, PATH_DELIMITER) < tokensSize(uri, PATH_DELIMITER)))
    {
        key_tokens = ft_split(key, PATH_DELIMITER);
        uri_tokens = ft_split(uri, PATH_DELIMITER);
        for (std::vector<std::string>::const_iterator it = key_tokens.begin(); it != key_tokens.end(); ++it)
        {
            if (uri_tokens.at(score).compare(*it) == 0)
                score++;
            else
                return (score);
        }
    }
    return (0);
}
location *Executor::getLongestMatchedLocation(HttpRequest &request, std::map<std::string, location *> map)
{
    std::string uri = request.getUri();
    int best_expected_score = tokensSize(uri, PATH_DELIMITER);
    location *best_match = NULL;
    int previous_score = 0;
    int score = -1;
    for (std::map<std::string, location *>::const_iterator it = map.begin(); it != map.end(); ++it)
    {
        score = matchedScore(uri, it->first);
        if (score > previous_score)
        {
            if (score == best_expected_score)
                return (it->second);
            best_match = it->second;
            previous_score = score;
        }
    }
    return (best_match);
}