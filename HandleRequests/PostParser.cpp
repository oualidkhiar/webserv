#include "PostParser.hpp"
#include "enums.hpp"
#include <fstream>

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

bool PostParser::saveUploadedFile(const std::string &uploadDir, const std::string &filename,
								  const std::string &content)
{
	std::string path = uploadDir;
	if (!path.empty() && path[path.size() - 1] != '/')
		path += '/';
	path += filename;
	std::ofstream file(path.c_str(), std::ios::binary | std::ios::trunc);
	if (!file.is_open())
		return false;
	if (!content.empty())
		file.write(&content[0], content.size());
	return file.good();
}

void PostParser::parseMultipartBody(HttpRequest &request, HttpResponse &response, const std::string &boundary)
{
	const std::vector<unsigned char> &bodyData = request.getBody().getBody();
	std::string bodyStr(bodyData.begin(), bodyData.end());

	std::string fullBoundary = "--" + boundary;

	size_t pos = 0;
	int filesUploaded = 0;

	while ((pos = bodyStr.find(fullBoundary, pos)) != std::string::npos)
	{
		pos += fullBoundary.size();
		if (bodyStr.substr(pos, 2) == "--")
			break;
		if (bodyStr.substr(pos, 2) == "\r\n")
			pos += 2;

		size_t nextBoundary = bodyStr.find(fullBoundary, pos);
		if (nextBoundary == std::string::npos)
			break;

		std::string thePart = bodyStr.substr(pos, nextBoundary - pos);

		size_t headerEnd = thePart.find("\r\n\r\n");
		if (headerEnd == std::string::npos)
		{
			pos = nextBoundary;
			continue;
		}

		std::string headers = thePart.substr(0, headerEnd);
		std::string content = thePart.substr(headerEnd + 4);

		if (content.size() >= 2 && content.substr(content.size() - 2) == "\r\n")
			content = content.substr(0, content.size() - 2);

		if (headers.find("filename=") != std::string::npos)
		{
			std::string filename = extractHeaderValue(headers, "filename=");

			if (!filename.empty())
			{
				std::string uploadDir = request.getLocation()->rootPath + "/" + request.getLocation()->upload_store;
				if (saveUploadedFile(uploadDir, filename, content))
					filesUploaded++;
				else
				{
					response.setStatus(HP_INTERNAL_SERVER_ERROR);
					response.setState(RESPONSE_FINISHED);
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

	response.setState(RESPONSE_FINISHED);
}

void PostParser::execute(HttpRequest &request, HttpResponse &response)
{
	std::string contentType = request.getHeader("content-type");
	if (contentType.find("multipart/form-data") != std::string::npos)
	{
		std::string boundary = extractBoundary(contentType);
		if (boundary.empty())
		{
			response.setStatus(HP_BAD_REQUEST);
			response.setState(RESPONSE_FINISHED);
			return;
		}
		if (request.getLocation() == NULL || request.getLocation()->upload_store.empty())
		{
			response.setStatus(HP_FORBIDDEN);
			response.setState(RESPONSE_FINISHED);
			return;
		}
		parseMultipartBody(request, response, boundary);
	}
	else if (contentType.find("application/octet-stream") != std::string::npos)
	{
		bool closeFile = (request.getStatus() == FINISHED);
		request.getFtFile()->writeToFile(request.getBody().getBody(), closeFile);
	}
	else
	{
		response.setStatus(HP_Unsupported_Media_Type);
		response.setState(RESPONSE_FINISHED);
	}
}

std::string PostParser::applicationFileName(HttpRequest &request)
{
	std::string value = request.getHeader("content-disposition");
	if (value.find("filename=") != std::string::npos)
	{
		std::string filename = extractHeaderValue(value, "filename=");
		if (!filename.empty())
			return (request.getLocation()->rootPath + "/" + request.getLocation()->upload_store + "/" + filename);
	}
	return "";
}