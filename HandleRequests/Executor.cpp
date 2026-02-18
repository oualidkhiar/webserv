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

Executor::Executor() {}

void Executor::executeDelete(HttpRequest &request, HttpResponse &response)
{
    std::string path = pathResolver(request);
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
    response.AddHeader("Connection", "Closed\r\n");
    response.AddHeader("server", "TestServer/1.1\r\n");
    response.setState(RESPONSE_FINISHED);
}

// added by saad -------------------------------------------------------------
std::string Executor::extractBoundary(const std::string &contentType)
{
    size_t pos = contentType.find("boundary=");
    if (pos == std::string::npos)
        return "";

    std::string boundary = contentType.substr(pos + 9);
    size_t end = boundary.find_first_of("; \t\r\n");
    if (end != std::string::npos)
        boundary = boundary.substr(0, end);

    return boundary;
}

std::string Executor::extractHeaderValue(const std::string &headers, const std::string &key)
{
    size_t pos = headers.find(key);
    if (pos == std::string::npos)
        return "";

    pos = headers.find("\"", pos);
    if (pos == std::string::npos)
        return "";

    size_t end = headers.find("\"", pos + 1);
    if (end == std::string::npos)
        return "";

    return headers.substr(pos + 1, end - pos - 1);
}

bool Executor::saveUploadedFile(const std::string &uploadDir, const std::string &filename,
                                const std::vector<unsigned char> &content)
{
    std::string fullPath = uploadDir;
    if (!fullPath.empty() && fullPath[fullPath.length() - 1] != '/')
        fullPath += "/";
    fullPath += filename;

    std::ofstream file(fullPath.c_str(), std::ios::binary | std::ios::trunc);
    if (!file.is_open())
        return false;

    file.write(reinterpret_cast<const char *>(&content[0]), content.size());
    file.close();

    return true;
}

void Executor::parseMultipartBody(HttpRequest &request, HttpResponse &response, const std::string &boundary)
{
    const std::vector<unsigned char> &bodyData = request.getBody().getBody();
    std::string bodyStr(bodyData.begin(), bodyData.end());

    std::string fullBoundary = "--" + boundary;
    std::string endBoundary = "--" + boundary + "--";

    size_t pos = 0;
    int filesUploaded = 0;

    while ((pos = bodyStr.find(fullBoundary, pos)) != std::string::npos)
    {
        pos += fullBoundary.length();

        if (bodyStr.substr(pos, 2) == "--")
            break;

        if (bodyStr.substr(pos, 2) == "\r\n")
            pos += 2;

        size_t nextBoundary = bodyStr.find(fullBoundary, pos);
        if (nextBoundary == std::string::npos)
            break;

        std::string part = bodyStr.substr(pos, nextBoundary - pos);

        size_t headerEnd = part.find("\r\n\r\n");
        if (headerEnd == std::string::npos)
        {
            pos = nextBoundary;
            continue;
        }

        std::string headers = part.substr(0, headerEnd);
        std::string content = part.substr(headerEnd + 4);

        if (content.size() >= 2 && content.substr(content.size() - 2) == "\r\n")
            content = content.substr(0, content.size() - 2);

        if (headers.find("filename=") != std::string::npos)
        {
            std::string filename = extractHeaderValue(headers, "filename=");

            if (!filename.empty())
            {
                // read upload directory from config file, if not set, use default "uploads"
                std::string uploadDir = request.getLocation()->upload_store;
                if (uploadDir.empty())
                    uploadDir = "temp";

                std::vector<unsigned char> fileContent(content.begin(), content.end());

                if (saveUploadedFile(uploadDir, filename, fileContent))
                    filesUploaded++;
                else
                {
                    response.setStatus(HP_INTERNAL_SERVER_ERROR);
                    return;
                }
            }
        }

        pos = nextBoundary;
    }

    if (filesUploaded > 0)
        response.setStatus(HP_CREATED);
    else
        response.setStatus(HP_OK);
}

void Executor::executePost(HttpRequest &request, HttpResponse &response)
{
    std::string contentType = request.getHeader("Content-Type");

    if (contentType.find("multipart/form-data") != std::string::npos)
    {
        std::string boundary = extractBoundary(contentType);

        if (boundary.empty())
        {
            response.setStatus(HP_BAD_REQUEST);
            return;
        }

        // the location must exist and have upload_store configured in the config file
        if (request.getLocation() == NULL || request.getLocation()->upload_store.empty())
        {
            response.setStatus(HP_FORBIDDEN);
            return;
        }

        parseMultipartBody(request, response, boundary);
    }
    else
    {
        response.setStatus(HP_OK);
    }
}
// added by saad -------------------------------------------------------------

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
    if (request.getLocation()->key == "/")
    {
        if (request.getLocation()->autoindex)
        {
            for (int i = 0; i < request.getLocation()->indexFiles.size(); i++)
            {
                path = (request.getLocation()->rootPath + "/" + request.getLocation()->indexFiles[i]);
                std::pair<int, FtFile *> p = extractFileInfos(path.c_str());
                if (p.first == 1)
                {
                    p.second->ft_close();
                    break;
                }
                path.clear();
            }
        }
    }
    else if (request.getLocation() != NULL)
    {
        if (request.getLocation()->rootPath.empty() == false)
            path = request.getLocation()->rootPath + request.getUri();
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

void Executor::execute(HttpRequest &request, HttpResponse &response, Cgi &c)
{
    setLocation(request);
    if (request.getLocation() == NULL)
    {
        response.setStatus(HP_NOT_FOUND);
        return;
    }
    if (!isAllowedMethod(request))
    {
        response.setStatus(HP_METHOD_NOT_ALLOWED);
        return;
    }
    if (request.getType() == DELETE)
        executeDelete(request, response);
    else if (request.getType() == POST)
        executePost(request, response);
    else if (request.getType() == GET)
    {
        executeGet(request, response);
        // c.executeCgi();
        // if (c.getResponseCode() != 0) {
        //     response with spicifique error
        //     response.setStatus(c.getResponseCode());
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
    std::vector<unsigned char> file_content; // not used ??
    std::string path = pathResolver(request);
    std::pair<int, FtFile *> pair;
    pair = extractFileInfos(path.c_str());
    if (pair.first != 1)
    {
        std::cout << pair.first << std::endl;
        response.setStatus(pair.first);
        // exit_error("Stat Eroor");
    }
    else
    {
        setContentTpe(response, path);
        response.setFile(pair.second);
    }
    response.setState(READING_LARGE_FILE);
    response.createBody();
}

int Executor::matchedScore(std::string uri, std::string key)
{

    // int score = 1;
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
    location *best_match = NULL;
    if (uri == "/")
    {
        best_match = map["/"];
        return best_match;
    }
    //  int best_expected_score = tokensSize(uri, PATH_DELIMITER);
    // std::cout<<"best_expected_score  = "<<best_expected_score<<std::endl;

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
    