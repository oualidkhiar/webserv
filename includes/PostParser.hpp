#pragma once
#include "HttpRequest.hpp"
#include "HttpResponse.hpp"

class PostParser
{
private:
	static bool readPartHeaders(HttpRequest &request, std::string &buf, const std::string &boundary);
	static bool writePartBody(HttpRequest &request, std::string &buf, const std::string &delimiter);
	static std::string generateFileName(HttpRequest &request);

public:
	static void executeUpload(HttpRequest &request, HttpResponse &response);
	static void executeCGI(HttpRequest &request);
	static void creatFile(HttpRequest &request);
	static std::string extractBoundary(const std::string &contentType);
};
