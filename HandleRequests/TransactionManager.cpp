#include "TransactionManager.hpp"
#include "RequestParser.hpp"
#include "Executor.hpp"

TransactionManager::TransactionManager() {}

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

void TransactionManager::appendToRequest(unsigned char *buffer, size_t size)
{
    RequestParser parser;
    this->request.appendRequestData(buffer, size);
    parser.create_request(request);
    if (request.getStatus() == FINISHED)
    {
        Executor executor;
        executor.execute(request);
    }
}

void TransactionManager::executeRequest()
{
    Executor execute;
    response = execute.execute(request);
}

void TransactionManager::readChunk()
{
    std::vector<unsigned char> chunk;
    chunk = response.getFile()->readFile();
    response.appendBodyToResponse(chunk);
}

std::pair<unsigned char *, size_t> TransactionManager::getResponse()
{

    if (getResponseState() == FRESH)
        executeRequest();
    if (getResponseState() == READING_LARGE_FILE)
        readChunk();
    return (response.getChunkFromRequest());
}
