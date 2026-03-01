#pragma once
#include "HttpRequest.hpp"
#include "HttpResponse.hpp"

class PostParser
{
private:
	static std::string extractBoundary(const std::string &contentType);
	static bool saveUploadedFile(const std::string &uploadDir, const std::string &filename,
								 const std::string &content);
	static void parseMultipartBody(HttpRequest &request, HttpResponse &response, const std::string &boundary);
	static std::string extractHeaderValue(const std::string &headers, const std::string &key);

public:
	static void executeUpload(HttpRequest &request, HttpResponse &response);
	static void executeCGI(HttpRequest &request);
	static std::string applicationFileName(HttpRequest &request);
	static void creatFile(HttpRequest &request, HttpResponse &response);
};
