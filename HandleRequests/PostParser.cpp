#include "PostParser.hpp"
#include "Executor.hpp"
#include "enums.hpp"
#include "utils.hpp"
#include "RequestParser.hpp"
#include <ctime>
#include <sstream>
#include <unistd.h>

std::string PostParser::extractBoundary(const std::string &contentType)
{
	size_t pos = contentType.find("boundary=");
	if (pos == std::string::npos)
		return "";

	pos += 9;
	while (pos < contentType.size() && (contentType[pos] == ' ' || contentType[pos] == '\t'))
		pos++;

	if (pos >= contentType.size())
		return "";

	size_t end = std::string::npos;

	if (contentType[pos] == '"')
	{
		pos++;
		end = contentType.find('"', pos);
		if (end != std::string::npos)
			return contentType.substr(pos, end - pos);
		return "";
	}

	size_t end_semi = contentType.find(';', pos);
	size_t end_space = contentType.find(' ', pos);
	size_t end_tab = contentType.find('\t', pos);
	size_t end_crlf = contentType.find("\r\n", pos);
	end = contentType.size();
	if (end_semi != std::string::npos && end_semi < end)
		end = end_semi;
	if (end_space != std::string::npos && end_space < end)
		end = end_space;
	if (end_tab != std::string::npos && end_tab < end)
		end = end_tab;
	if (end_crlf != std::string::npos && end_crlf < end)
		end = end_crlf;
	return contentType.substr(pos, end - pos);
}

std::string PostParser::extractHeaderValue(const std::string &headers, const std::string &key)
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

bool PostParser::readPartHeaders(HttpRequest &request, std::string &buf, const std::string &boundary)
{
	if (buf.size() >= boundary.size() + 2 && buf.compare(0, boundary.size(), boundary) == 0)
	{
		if (buf[boundary.size()] == '\r' && buf[boundary.size() + 1] == '\n')
			buf.erase(0, boundary.size() + 2);
		else
			return false;
	}
	else if (buf.size() < boundary.size() + 2)
		return false;

	size_t hEnd = buf.find("\r\n\r\n");
	if (hEnd == std::string::npos)
		return false;

	std::string headers = buf.substr(0, hEnd);
	buf.erase(0, hEnd + 4);

	std::string filename = extractHeaderValue(headers, "filename=");
	if (!filename.empty())
	{
		std::string path = request.getLocation()->rootPath + "/"
					 + request.getLocation()->upload_store + "/" + filename;
		request.setFtFile(new FtFile(path));
	}
	request.setMpState(MP_WRITING_BODY);
	return true;
}

bool PostParser::writePartBody(HttpRequest &request, HttpResponse &response, std::string &buf, const std::string &delimiter)
{
	size_t bPos = buf.find(delimiter);
	if (bPos != std::string::npos)
	{
		if (request.getFtFile())
		{
			std::vector<unsigned char> last(buf.begin(), buf.begin() + bPos);
			if (request.getFtFile()->writeToFile(last, true) == -1)
			{
				response.setStatus(HP_INTERNAL_SERVER_ERROR);
				response.setState(RESPONSE_FINISHED);
				return false;
			}
			delete request.getFtFile();
			request.setFtFile(NULL);
		}
		size_t after = bPos + delimiter.size();
		// "--boundary--" final boundary, we're done
		if (after + 2 <= buf.size() && buf[after] == '-' && buf[after + 1] == '-')
		{
			response.setStatus(HP_CREATED);
			response.setState(RESPONSE_FINISHED);
			buf.clear();
			return false;
		}
		// "--boundary\r\n" another part follows
		if (after + 2 <= buf.size() && buf[after] == '\r' && buf[after + 1] == '\n')
			after += 2;
		buf.erase(0, after);
		request.setMpState(MP_READING_HEADERS);
		return true;
	}
	size_t keep = delimiter.size() - 1;
	if (buf.size() > keep)
	{
		size_t safe = buf.size() - keep;
		if (request.getFtFile())
		{
			std::vector<unsigned char> chunk(buf.begin(), buf.begin() + safe);
			if (request.getFtFile()->writeToFile(chunk, false) == -1)
			{
				response.setStatus(HP_INTERNAL_SERVER_ERROR);
				response.setState(RESPONSE_FINISHED);
				return false;
			}
		}
		buf.erase(0, safe);
	}
	return false;
}

void PostParser::executeUpload(HttpRequest &request, HttpResponse &response)
{
	std::string contentType = request.getHeader("content-type");

	if (contentType.find("multipart/form-data") != std::string::npos)
	{
		const std::vector<unsigned char> &body = request.getBody().getBody();
		std::string &buf = request.getMpBuffer();
		buf.append(body.begin(), body.end());
		request.getBody().clearBody();

		const std::string &boundary = request.getBoundary();  // "--boundary"
		const std::string delimiter  = "\r\n" + boundary;     // "\r\n--boundary"

		bool loop = true;
		while (loop)
		{
			loop = false;
			if (request.getMpState() == MP_READING_HEADERS)
				loop = readPartHeaders(request, buf, boundary);
			else
				loop = writePartBody(request, response, buf, delimiter);
			if (response.getState() == RESPONSE_FINISHED)
				return;
		}
		// All body data received but no closing boundary found — malformed multipart
		if (request.getStatus() == FINISHED && response.getState() != RESPONSE_FINISHED)
		{
			response.setStatus(HP_BAD_REQUEST);
			response.setState(RESPONSE_FINISHED);
		}
	}
	else
	{
		bool closeFile = (request.getStatus() == FINISHED);
		if (request.getFtFile()->writeToFile(request.getBody().getBody(), closeFile) == -1) { // write syscall failed
			response.setState(RESPONSE_FINISHED);
			response.setStatus(HP_INTERNAL_SERVER_ERROR);
			request.setStatus(FINISHED);
		}
	}
}


static std::string generateDateName(const std::string &basePath)
{
	char buf[30];
	std::time_t todayTime = std::time(NULL);
	std::strftime(buf, sizeof(buf), "upload_%Y-%m-%d_%H-%M-%S", std::localtime(&todayTime));
	std::string name = buf;

	if (access((basePath + name).c_str(), F_OK) != 0)
		return name;
	int counter = 1;
	while (true)
	{
		std::ostringstream oss;
		oss << name << "_" << counter++;
		std::string name_n = oss.str();
		if (access((basePath + name_n).c_str(), F_OK) != 0)
			return name_n;
	}
}

std::string PostParser::applicationFileName(HttpRequest &request)
{
	std::string basePath = request.getLocation()->rootPath + "/" + request.getLocation()->upload_store + "/";
	std::string value = request.getHeader("content-disposition");
	if (value.find("filename=") != std::string::npos)
	{
		std::string filename = extractHeaderValue(value, "filename=");
		if (!filename.empty())
			return (basePath + filename);
	}
	return (basePath + generateDateName(basePath));
}

void PostParser::executeCGI(HttpRequest &request)
{
	//TODO validate path and allowed methods .
	bool closeFile = (request.getStatus() == FINISHED);
	request.getFtFile()->writeToFile(request.getBody().getBody(), closeFile);
	request.getBody().clearBody();
}

void PostParser::creatFile(HttpRequest &request, HttpResponse &response)
{
	if (request.getCGIType() != NO_CGI)
	{
		std::string name = "/tmp/" + generateRandomName();
		FtFile *file = new FtFile(name);
		request.setFtFile(file);
	}
	if (request.getCGIType() == NO_CGI)
	{
		std::string contentType = request.getHeader("content-type");
		if (!contentType.empty() && contentType.find("multipart/form-data") != std::string::npos)
			return;

		std::string filePath = PostParser::applicationFileName(request);
		FtFile *file = new FtFile(filePath);
		request.setFtFile(file);
	}
}