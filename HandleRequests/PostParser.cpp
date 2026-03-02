#include "PostParser.hpp"
#include "Executor.hpp"
#include "enums.hpp"
#include "utils.hpp"
#include "RequestParser.hpp"

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
			{
				if (buf.size() >= boundary.size() + 2
					&& buf.compare(0, boundary.size(), boundary) == 0)
				{
					if (buf[boundary.size()] == '\r' && buf[boundary.size() + 1] == '\n')
						buf.erase(0, boundary.size() + 2);
					else
						break; 
				}
				else if (buf.size() < boundary.size() + 2)
					break; 

				size_t hEnd = buf.find("\r\n\r\n");
				if (hEnd == std::string::npos)
					break; 

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
				loop = true;
			}
			else
			{
				size_t bPos = buf.find(delimiter);
				if (bPos != std::string::npos)
				{
					if (request.getFtFile())
					{
						std::vector<unsigned char> last(buf.begin(), buf.begin() + bPos);
						request.getFtFile()->writeToFile(last, true);
						delete request.getFtFile();
						request.setFtFile(NULL);
					}
					size_t after = bPos + delimiter.size();
					// "--boundary--" y3ni final boundary, we're done
					if (after + 2 <= buf.size() && buf[after] == '-' && buf[after + 1] == '-')
					{
						response.setStatus(HP_CREATED);
						response.setState(RESPONSE_FINISHED);
						buf.clear();
						return;
					}
					// "--boundary\r\n"for  another part follows, just skip  the "\r\n"
					if (after + 2 <= buf.size() && buf[after] == '\r' && buf[after + 1] == '\n')
						after += 2;
					buf.erase(0, after);
					request.setMpState(MP_READING_HEADERS);
					loop = true;
				}
				else
				{
					size_t keep = delimiter.size() - 1;
					if (buf.size() > keep)
					{
						size_t safe = buf.size() - keep;
						if (request.getFtFile())
						{
							std::vector<unsigned char> chunk(buf.begin(), buf.begin() + safe);
							request.getFtFile()->writeToFile(chunk, false);
						}
						buf.erase(0, safe);
					}
				}
			}
		}
		// All body data received but closing boundary was already processed
		if (request.getStatus() == FINISHED && response.getState() != RESPONSE_FINISHED)
		{
			response.setStatus(HP_CREATED);
			response.setState(RESPONSE_FINISHED);
		}
	}
	else if (contentType.find("application/octet-stream") != std::string::npos)
	{
		bool closeFile = (request.getStatus() == FINISHED);
		if (request.getFtFile()->writeToFile(request.getBody().getBody(), closeFile) == -1) { // write syscall failed
			response.setState(RESPONSE_FINISHED);
			response.setStatus(HP_INTERNAL_SERVER_ERROR);
			request.setStatus(FINISHED);
		}
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
	return (basePath + generateRandomName());
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