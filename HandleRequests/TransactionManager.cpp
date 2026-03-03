#include "../HandleRequests/ErrorResponse.hpp"
#include "TransactionManager.hpp"
#include "RequestParser.hpp"
#include "Executor.hpp"
#include <cstring>

TransactionManager::TransactionManager() : c(request, response)
{
    responsed = false;
}

std::pair<unsigned char *, size_t> TransactionManager::getRoofResponse()
{
    std::pair<unsigned char *, size_t> pair;
    std ::string roof;
    unsigned char *roofBuffer;
    roof = response.getHeaderLine();
    for (std::map<std::string, std::string>::const_iterator it = response.headersBegin(); it != response.headersEnd(); it++) {
        roof = roof + it->first + ": " + it->second;
    }
    roof += "\r\n";
    pair.second = roof.length();
    roofBuffer = new unsigned char[pair.second];
    std::copy(roof.c_str(), roof.c_str() + pair.second, roofBuffer);
    pair.first = roofBuffer;


    return (pair);
}

std::pair<unsigned char *, size_t> TransactionManager::joinPairs(std::pair<unsigned char *, size_t> &pair1, std::pair<unsigned char *, size_t> &pair2)
{

    if (pair1.first == NULL || pair2.first == NULL)
        std::cout << "NULL YA DINK" << std::endl;
    std::pair<unsigned char *, size_t> joined_pair;
    size_t newSize = pair1.second + pair2.second;
    unsigned char *joined_buffer = new unsigned char[newSize];
    std::copy(pair1.first, pair1.first + pair1.second, joined_buffer);
    std::copy(pair2.first, pair2.first + pair2.second, joined_buffer + pair1.second);
    delete[] pair1.first;
    delete[] pair2.first;
    pair1.first = NULL;
    pair1.second = 0;
    pair2.first = NULL;
    pair2.second = 0;
    joined_pair.first = joined_buffer;
    joined_pair.second = newSize;

    return joined_pair;
}

RequestType TransactionManager::getRequestType()
{
    return (this->request.getType());
}

status TransactionManager::getRequestStatus()
{
    return (this->request.getStatus());
}

ResponseState TransactionManager::getResponseState()
{
    return (this->response.getState());
}

int TransactionManager::getResponseCode()
{
    return (this->response.getStatus());
}

void TransactionManager::appendToRequest(unsigned char *buffer, size_t size)
{
    this->request.appendRequestData(buffer, size, response);
}

void TransactionManager::executeRequest()
{
    Executor execute;
    execute.execute(request, response);
    if (execute.getExecutorCase() == CGI_EXECUTION) {
        c.executeCgi();
		if (c.getResponseCode() != 0) {
			response.setState(RESPONSE_FINISHED);
			response.setStatus(c.getResponseCode());
		}
    }
}

void TransactionManager::readChunk()
{
    std::vector<unsigned char> chunk;
    chunk = response.getFile()->readFile();
    response.appendBodyToResponse(chunk);
    if (response.getFile()->getState() == FILE_FINISHED)
        response.setState(RESPONSE_FINISHED);
}

void TransactionManager::setServer(serverConfig *config)
{
    this->request.setConfig(config);
}

std::pair<unsigned char *, size_t> TransactionManager::firstResponse()
{
    response.initializeResponse();
    responsed = true;
    std::pair<unsigned char *, size_t> roofResponse = getRoofResponse();
    std::pair<unsigned char *, size_t> body = response.getChunkFromRequest();
    if (body.second == 0) {
        return roofResponse;
    }
    return (joinPairs(roofResponse, body));
}

std::pair<unsigned char *, size_t> TransactionManager::getResponse()
{
    if (getResponseState() == FRESH) {
        executeRequest();
    }
    if (getResponseState() == WAITING_FOR_CGI)
    {
        c.isChildFinishExecute_Cgi();
        if (getResponseState() == WAITING_FOR_CGI)
        {
            return std::make_pair((unsigned char *)"", 0); // if child proccess still running cgi don't do anything go handle the other clients
        }
    }
    if (response.getStatus() != 0 and response.getStatus() != 301 and response.getStatus() != 302) {
        this->response.setState(RESPONSE_FINISHED);
        std::pair<unsigned char *, size_t> p = ErrorResponse::getErrorResponse(response.getStatus());
        return p;
    }
    else if (getResponseState() == READING_LARGE_FILE) {
        readChunk();
    }
    if (responsed == false)
        return (firstResponse());
    else
        return (response.getChunkFromRequest());

    std::cout << "NO CONDTION IS TRUE IN TransactionManager::getResponse" << std::endl;
}
