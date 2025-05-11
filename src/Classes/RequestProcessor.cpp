/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   RequestProcessor.cpp                              :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: zouddach <zouddach@student.1337.ma>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/02/06 16:46:10 by zouddach          #+#    #+#             */
/*   Updated: 2025/04/17 21:01:29 by zouddach         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "RequestProcessor.hpp"

int CURRENT_RUNNING_CGI = 0;

vector<string> split(string str, char delimiter);
string trim(const string &str);
vector<string> splitByString(string str, string delimiter);

RequestProcessor::RequestProcessor() {
    init_dangerousePatterns();
    _headers_parsed = false;
    _content_length = 0;
    _body_size = 0;
    _responded = false;
    _received = false;
    fd = -1;
    _file = NULL;
    _server = NULL;
    _cgi = NULL;
	_status = 200;
	_friend_server = false;
}

RequestProcessor::RequestProcessor(const RequestProcessor &req) {
    init_dangerousePatterns();
    _headers_parsed = false;
    _content_length = 0;
    _body_size = 0;
    _responded = false;
    _received = false;
    fd = -1;
    _file = NULL;
    _server = NULL;
    _cgi = NULL;
	_friend_server = false;
	_status = req._status;
	_request = req.getRequest();
	_method = req.getMethod();
	_uri = req.getUri();
	_host = req.getHost();
	_port = req.getPort();
	_connection = req.getConnection();
	_content_length = req.getContentLength();
	_body = req.getBody();
	_query = req.getQuery();
	_cookie = req.getCookie();
	_formFields = req.getFormFields();
	_fileContentType = req.getFileContentType();
	_fileContent = req.getFileContent();
}

RequestProcessor::~RequestProcessor() {
	if (_file)
		delete _file;
	if (_cgi)
		delete _cgi;
	if (fd != -1)
		close(fd);
	if (_fileStream.is_open())
	    _fileStream.close();

	if (_friend_server)
	{
		delete _server;
	}

}

string RequestProcessor::getRequest() const
{
	return _request;
}

string RequestProcessor::getMethod() const
{
	return _method;
}

string RequestProcessor::getUri() const
{
	return _uri;
}

string RequestProcessor::getHost() const
{
	return _host;
}

string RequestProcessor::getPort() const
{
	return _port;
}

string RequestProcessor::getConnection() const
{
	return _connection;
}

size_t RequestProcessor::getContentLength() const
{
	return _content_length;
}

string RequestProcessor::getBody() const
{
	return _body;
}

string RequestProcessor::getQuery() const
{
	return _query;
}

string RequestProcessor::getCookie() const
{
	return _cookie;
}

void    RequestProcessor::setReceived(bool received)
{
    _received = received;
}

bool RequestProcessor::received() const
{
    return _received;
}

void    RequestProcessor::setResponded(bool responded)
{
    _responded = responded;
}

bool RequestProcessor::responded() const
{
    return _responded;
}

void RequestProcessor::setResponseToSend(string response)
{
    _responseToSend = response;
}

string RequestProcessor::getResponseToSend() const
{
    return _responseToSend;
}

string RequestProcessor::getContentType() const
{
    return _content_type;
}

string RequestProcessor::getAuthorization() const
{
    return _authorization;
}

RequestProcessor&    RequestProcessor::operator=(const RequestProcessor &req)
{
    if (this != &req)
    {
        _request = req.getRequest();
        _method = req.getMethod();
        _uri = req.getUri();
        _host = req.getHost();
        _port = req.getPort();
        _connection = req.getConnection();
        _content_length = req.getContentLength();
        _body = req.getBody();
        _query = req.getQuery();
        _cookie = req.getCookie();
        _formFields = req.getFormFields();
        _fileContentType = req.getFileContentType();
        _fileContent = req.getFileContent();

        _cgi = req._cgi;
        _file = req._file;
        _server = req._server;
        _route = req._route;
    }
    return *this;
}

string RequestProcessor::generateHttpHeaders(Server *server, int status_code, long fileSize)
{
    string _Http_headers;
    (void)server;
    _status = status_code;
    _Http_headers = "HTTP/1.1 " + cpp11_toString(status_code) + getStatusMessage(status_code);
    _Http_headers += "Server: webserv/1.0.0 (Ubuntu)\r\n";
    _Http_headers += "Content-Type: " + (status_code >= 200 && status_code < 300 ? generateContentType() : "text/html") + "\r\n";

        _Http_headers += "Content-Length: " + cpp11_toString(fileSize) + "\r\n";
    if (this->getConnection() == "close")
        _Http_headers += "Connection: close\r\n";
    else
        _Http_headers += "Connection: keep-alive\r\n";
    _Http_headers += "\r\n";

    return _Http_headers;
}
string RequestProcessor::getStatusMessage(int status_code) const
{
    switch (status_code)
    {
        case 200: return " OK\r\n";
        case 403: return " Forbidden\r\n";
        case 400: return " Bad Request\r\n";
        case 405: return " Method Not Allowed\r\n";
        case 404: return " Not Found\r\n";
        case 500: return " Internal Server Error\r\n";
        case 501: return " Not Implemented\r\n";
        case 502: return " Bad Gateway\r\n";
        case 503: return " Service Unavailable\r\n";
        case 301: return " Moved Permanently\r\n";
        case 302: return " Found\r\n";
        case 303: return " See Other\r\n";
        case 307: return " Temporary Redirect\r\n";
        case 308: return " Permanent Redirect\r\n";
        case 413: return " Request Entity Too Large\r\n";
        case 408: return " Request Timeout\r\n";
		case 411: return " Length Required\r\n";
        default: return " Unknown Status\r\n";
    }
}

File *RequestProcessor::GetFile(string path) const {
    if (access(path.c_str(), F_OK) == -1 || access(path.c_str(), R_OK) == -1)
        return NULL;
	else
    	return new File(path);
}

string  RequestProcessor::createDirectoryListing(const string& path) const {
    string htmlContent = "<html><head><title>Directory Listing</title></head><body><h1>Directory Listing</h1><ul>";
    DIR* dir = opendir(path.c_str());
    if (dir != NULL) {
        struct dirent* entry;
        while ((entry = readdir(dir)) != NULL) {
            string name = entry->d_name;
                htmlContent += "<li><a href=\"";
                htmlContent += name;
                htmlContent += "\">";
                htmlContent += name;
                htmlContent += "</a></li>";
        }
        closedir(dir);
    }
    htmlContent += "</ul></body></html>";
    return htmlContent;
}

File* RequestProcessor::handleDirectory(const string& path) const {
    string htmlContent = createDirectoryListing(path);
    string tempPath = "/tmp/dirlist_" + cpp11_toString(time(NULL)) + ".html";
    ofstream tempFile(tempPath.c_str());
    if (!tempFile.is_open())
        return NULL;

    tempFile << htmlContent;
    tempFile.close();
    return new File(tempPath);
}

string     RequestProcessor::ReturnServerErrorPage(Server *server, int status_code)
{
    string error_page_path;
    if (server == NULL)
        error_page_path = "./error/404.html";
    else
        error_page_path = server->getErrorPage(status_code);
    if (error_page_path.empty()) {
        if (status_code == 403)
            error_page_path = "./error/403.html";
        else if (status_code == 404)
            error_page_path = "./error/404.html";
        else if (status_code == 500)
            error_page_path = "./error/500.html";

    }
    File error_page(error_page_path);
    std::string response = generateHttpHeaders(server, status_code, error_page.getSize());
    response += error_page.getData();
    return response;
}

string RequestProcessor::processIndexFiles(vector<string> &indexFiles) const {
    for (size_t i = 0; i < indexFiles.size(); i++) {
        string indexPath = WORKIN_PATH + indexFiles[i];
        if (access(indexPath.c_str(), F_OK) == 0) {
            return indexPath;
        }
    }
    return "";
}

string RequestProcessor::checkRedirectionFile(string path) {
    struct stat pathStat;
    string pathToCheck = WORKIN_PATH + this->_server->getRoot() + path.substr(1);
    if (stat(pathToCheck.c_str(), &pathStat) == -1) {
        return "";
    }
    if (S_ISREG(pathStat.st_mode)) {
        return pathToCheck;
    }
    return "";
}

File* RequestProcessor::GETResponse(string root, string requestedPath) {
    string reqUri = this->getUri();
    bool hasRoot = false;
    string basePath;
    if (_route->getRouteRoot().empty()) {
        basePath = WORKIN_PATH + root + reqUri.substr(1);
    }
    else {

        string routeName = _route->getRouteName();
        string uriWithoutRouteName;

        if (routeName != "/")
            uriWithoutRouteName = cpp11_replace(reqUri, cpp11_replace(routeName, "\"", ""), "");
        else
            uriWithoutRouteName = reqUri.substr(1);
        basePath = WORKIN_PATH + root + _route->getRouteRoot() + uriWithoutRouteName;
        hasRoot = true;
        (void)hasRoot;
    }
    if (requestedPath == "/") {
        if (_route->getRedirectUrl().second != -1) {
            string newPath = checkRedirectionFile(_route->getRedirectUrl().first);
            if (newPath.empty() == false) {
                _status = _route->getRedirectUrl().second;
                return GetFile(newPath);
            }
            if (!_server->isRouteExist(_route->getRedirectUrl().first)) {
                return NULL;
            }
            else
            {
                _status = _route->getRedirectUrl().second;
                _route = _server->getRouteFromUri(_route->getRedirectUrl().first);
            }
        }
        vector<string> indexFiles = _route->getRouteIndexFiles();
        basePath = processIndexFiles(indexFiles);
        if (basePath.empty() && _route->getRouteDirectoryListing()) {
            _status = 200;
            return handleDirectory(WORKIN_PATH + root);
        }
        else if (basePath.empty()) {
            return NULL;
        }
        else {
            _status = (_status >= 300 && _status < 400) ? _status : 200;
            return GetFile(basePath);
        }
    }
    if (_route->getRedirectUrl().second != -1) {
        string newRouteName = _route->getRedirectUrl().first;
        int newStatusCode = _route->getRedirectUrl().second;

        string newPath = checkRedirectionFile(newRouteName);
        if (newPath.empty() == false) {
            _status = _route->getRedirectUrl().second;
            return GetFile(newPath);
        }
        if (!_server->isRouteExist(newRouteName)) {
            return NULL;
        }
        else {
            _route = _server->getRouteFromUri(newRouteName);
            _status = newStatusCode;
        }
    }
    struct stat pathStat;
    if (stat(basePath.c_str(), &pathStat) == -1) {
        return NULL;
    }
    if (S_ISREG(pathStat.st_mode)) {
        _status = 200;
        return GetFile(basePath);
    }
    if (S_ISDIR(pathStat.st_mode)) {
        vector<string> indexFiles = _route->getRouteIndexFiles();
        string indexPath = processIndexFiles(indexFiles);
        if (!indexPath.empty()) {
            _status = (_status >= 300 && _status < 400) ? _status : 200;
            return GetFile(indexPath);
        }
        if (_route->getRouteDirectoryListing()) {
            _status = (_status >= 300 && _status < 400) ? _status : 200;
            return handleDirectory(basePath);
        }
    }
    return NULL;
}

void RequestProcessor::debugRequest() const
{
    cerr << "==== REQUEST DEBUG ====" << endl;
    cerr << "Method: " << _method << endl;
    cerr << "URI: " << _uri << endl;
    cerr << "Query: " << _query << endl;
    cerr << "Host: " << _host << endl;
    cerr << "Content-Type: " << _content_type << endl;
    cerr << "Content-Length: " << _content_length << endl;

    if (!_filename.empty())
    {
        cerr << "Uploaded filename: " << _filename << endl;
        cerr << "File content type: " << _fileContentType << endl;
        cerr << "File content length: " << _fileContent.length() << " bytes" << endl;
    }

    cerr << "Form fields: " << endl;
    for (map<string, string>::const_iterator it = _formFields.begin(); it != _formFields.end(); ++it)
    {
        cerr << "  " << it->first << ": ";
        if (it->second.length() > 50)
            cerr << it->second.substr(0, 50) << "... (" << it->second.length() << " bytes)";
        else
            cerr << it->second;
        cerr << endl;
    }
    cerr << "======================" << endl;
}

void RequestProcessor::parseFormUrlEncoded(const string &body)
{
    vector<string> pairs = split(body, '&');
    for (size_t i = 0; i < pairs.size(); i++)
	{
		string pair = pairs[i];
		size_t equalsPos = pair.find('=');
		if (equalsPos != string::npos)
		{
			string key = pair.substr(0, equalsPos);
			string value = pair.substr(equalsPos + 1);

			_formFields[key] = value;
		}
	}
}

RequestProcessor::RequestProcessor(string req, string __port, Server *server)
{
    _request = req;
    _server = server;
    _port = __port;
    _responded = false;

    if (this->parseHeaders(req))
        cerr << "Error parsing headers" << endl;
    if (this->parseBody(req))
        cerr << "Error parsing body" << endl;
}

int    RequestProcessor::parseHeaders(string req) {
	bool content_length_set = false;
    size_t headerEnd = req.find("\r\n\r\n");
    if (headerEnd == string::npos) {
        headerEnd = req.find("\n\n");
        if (headerEnd == string::npos) {
            cerr << "Invalid request format: couldn't find header/body separator" << endl;
            return (BAD_REQUEST_STATUS_CODE);
        }
    }

    string headersSection = req.substr(0, headerEnd);

    string normalizedHeaders = headersSection;
    size_t pos = 0;
    while ((pos = normalizedHeaders.find("\r\n", pos)) != string::npos) {
        normalizedHeaders.replace(pos, 2, "\n");
        pos++;
    }

    vector<string> headers = split(normalizedHeaders, '\n');
    if (headers.empty()) {
        cerr << "Invalid request: no headers found" << endl;
        return (BAD_REQUEST_STATUS_CODE);
    }

    vector<string> requestLine = splitByString(headers[0], " ");
    if (requestLine.size() < 2 || requestLine.size() > 3) {
        return (BAD_REQUEST_STATUS_CODE);
    }

    _method = requestLine[0];
    if (_method != "GET" && _method != "POST" && _method != "DELETE") {
        return (NOT_ALLOWED_STATUS_CODE);
    }
    _uri = requestLine[1];

    size_t queryPos = _uri.find('?');
    if (queryPos != string::npos) {
        _query = _uri.substr(queryPos + 1);
        _uri = _uri.substr(0, queryPos);
    }

    for (size_t i = 1; i < headers.size(); i++) {
        string header = headers[i];
        size_t colonPos = header.find(':');
        if (colonPos != string::npos) {
            string key = trim(header.substr(0, colonPos));
            string value = trim(header.substr(colonPos + 1));

            if (key == "Host") {
                size_t portPos = value.find(':');
                if (portPos != string::npos) {
                    _host = trim(value.substr(0, portPos));
                } else {
                    _host = value;
                }
            } else if (key == "Content-Type") {
                _content_type = value;
            } else if (key == "Content-Length") {
				content_length_set = true;
                _content_length = std::atoi(value.c_str());
                Route *route = _server->getRouteFromUri(_uri);
                if (_content_length > route->getClientMaxBodySize() && route->getClientMaxBodySize() != (size_t)-1) {
                    cerr << "Content-Length exceeds max body size" << endl;
                    return (REQUEST_ENTITY_TOO_LARGE_STATUS_CODE);
                }
            } else if (key == "Connection") {
                _connection = value;
            } else if (key == "Cookie") {
                _cookie = value;
            } else if (key == "Authorization") {
                _authorization = value;
            }
            else if (key == "Transfer-Encoding") {
                _transfere_encoding = value;
            }
        }
    }

    Server *server = this->_server;
    if (server->getFriends().size() > 0)
    {
        for ( size_t k = 0; k < server->getFriends().size(); k++)
        {
            Server *ss = new Server(server->getFriends()[k]);
            vector<string> ss_names = ss->getServerNames();
            for (size_t j = 0; j < ss_names.size(); j++)
            {
                if (ss_names[j] == this->getHost())
                {
                    this->_server = ss;
					_friend_server = true;
                    break ;
                }
            }
            delete ss;
        }
    }

	if (!content_length_set && _method == "POST" && _transfere_encoding != "chunked")
		return (REQUEST_LENGTH_REQUIRED_STATUS_CODE);

    return (OK_STATUS_CODE);
}

string RequestProcessor::DELETEResponse(string root, string requestedPath)  {
    string routeName = _route->getRouteName();
    string reqUri = this->getUri();
    string basePath = WORKIN_PATH + root + reqUri.substr(1);
    if (requestedPath == "/") {
        return this->generateHttpHeaders(_server, FORBIDDEN_STATUS_CODE, 0);
    }
    struct stat pathStat;
    if (stat(basePath.c_str(), &pathStat) == -1)
        return this->generateHttpHeaders(_server, NOT_FOUND_STATUS_CODE, 0);

    if (S_ISREG(pathStat.st_mode)) {
        if (remove(basePath.c_str()) == 0) {
            return this->generateHttpHeaders(_server, OK_STATUS_CODE, 0);
        } else {
            return this->generateHttpHeaders(_server, FORBIDDEN_STATUS_CODE, 0);
        }
    }

    if (S_ISDIR(pathStat.st_mode)) {
            return this->generateHttpHeaders(_server, FORBIDDEN_STATUS_CODE, 0);
    }
    return this->generateHttpHeaders(_server, OK_STATUS_CODE, 0);
}

int RequestProcessor::handleTransferEncodingChunked(string &body) {
    stringstream ss(body);
    body.clear();
    string chunk;
    while (getline(ss, chunk)) {
        if (chunk == "0") {
            break;
        }
        size_t chunkSize = strtol(chunk.c_str(), NULL, 16);
        if (chunkSize == 0) {
            break;
        }
        string chunkData;
        getline(ss, chunkData);
        _body += chunkData.substr(0, chunkSize);
    }
    return 0;
}

int RequestProcessor::parseBody(string req) {
        size_t headerEnd = req.find("\r\n\r\n");
        if (headerEnd == string::npos) {
            headerEnd = req.find("\n\n");
            if (headerEnd == string::npos) {
                cerr << "Invalid request format: couldn't find header/body separator" << endl;
                return (1);
            }
        }

        string headersSection = req.substr(0, headerEnd);

        _body = "";
        if (headerEnd + 4 < req.length()) {
            _body = req.substr(headerEnd + 4);
        } else if (headerEnd + 2 < req.length()) {
            _body = req.substr(headerEnd + 2);
        }

        if (_transfere_encoding == "chunked")
            return handleTransferEncodingChunked(_body);

        if (_content_type.find("application/x-www-form-urlencoded") != string::npos) {
            parseFormUrlEncoded(_body);
        } else if (_content_type.find("multipart/form-data") != string::npos) {
            size_t boundaryPos = _content_type.find("boundary=");
            if (boundaryPos != string::npos) {
                string boundary = _content_type.substr(boundaryPos + 9);
                if (boundary[0] == '"' && boundary[boundary.length()-1] == '"') {
                    boundary = boundary.substr(1, boundary.length() - 2);
                }
                processMultipartFormData(boundary);
            }
        }
        else if (_content_type.find("text/plain") != string::npos) {
            parseTextPlainUpload(_body);
        }
        else if (_content_type.find("application/json") != string::npos) {
            _fileContent = _body;
            _filename = "upload_" + cpp11_toString(time(NULL)) + ".json";
            _fileContentType = "application/json";
        }
    return (0);
}

string RequestProcessor::getFileContentType() const
{
    return _fileContentType;
}

bool RequestProcessor::processMultipartFormData(const std::string& boundary) {
    std::string completeBoundary = "--" + boundary;
    std::string endBoundary = "--" + boundary + "--";

    size_t pos = 0;
    while (pos < _body.size()) {
        size_t boundaryPos = _body.find(completeBoundary, pos);

        if (boundaryPos == std::string::npos) {
            size_t safeReadLimit = _body.size() - completeBoundary.length();
            if (pos < safeReadLimit) {
                if (_fileStream && _fileStream.is_open()) {
                    _fileStream.write(&_body[pos], safeReadLimit - pos);
                }
                pos = safeReadLimit;
            }
            break;
        }

        if (pos > 0 && _fileStream.is_open()) {
            _fileStream.write(&_body[pos], boundaryPos - pos);
        }

        pos = boundaryPos + completeBoundary.length();

        if (_body.substr(boundaryPos, endBoundary.length()) == endBoundary) {
            pos = boundaryPos + endBoundary.length();
            break;
        }

        if (pos + 2 <= _body.size() && _body[pos] == '\r' && _body[pos+1] == '\n') {
            pos += 2;
        }

        std::string filename;
        std::string contentType;

        size_t headerEnd = _body.find("\r\n\r\n", pos);
        if (headerEnd == std::string::npos) {
            break;
        }

        std::string headers = _body.substr(pos, headerEnd - pos);
        pos = headerEnd + 4;

        size_t filenamePos = headers.find("filename=\"");
        if (filenamePos != std::string::npos) {
            filenamePos += 10;
            size_t filenameEnd = headers.find("\"", filenamePos);
            if (filenameEnd != std::string::npos) {
                filename = headers.substr(filenamePos, filenameEnd - filenamePos);

                if (_fileStream && _fileStream.is_open()) {
                    _fileStream.close();
                }

                if (_fileStream && _method == "POST") {
                    Route *route = _server->getRouteFromUri(getUri());
                    if (filename.empty()) {
                        filename = "upload_" + cpp11_toString(time(NULL)) + ".bin";
                    }
                    string store_path = "./body/" + filename;
                    if (!route->getUploadStore().empty())
                        store_path = WORKIN_PATH + route->getUploadStore() + filename;
                    _fileStream.open(store_path.c_str(), std::ios::binary);
                }
            }
        }

        size_t nextBoundaryPos = _body.find(completeBoundary, pos);
        if (nextBoundaryPos == std::string::npos) {
            size_t safeReadLimit = _body.size() - completeBoundary.length();
            if (pos < safeReadLimit && _fileStream.is_open()) {
                _fileStream.write(&_body[pos], safeReadLimit - pos);
            }
            pos = safeReadLimit;
            break;
        } else {
            if (_fileStream && _fileStream.is_open()) {
                size_t contentEnd = nextBoundaryPos;
                if (contentEnd >= 2 && _body[contentEnd-2] == '\r' && _body[contentEnd-1] == '\n') {
                    contentEnd -= 2;
                }

                if (contentEnd > pos) {
                    _fileStream.write(&_body[pos], contentEnd - pos);
                }
            }
            pos = nextBoundaryPos;
        }
    }

    if (pos < _body.size()) {
        _body = _body.substr(pos);
        return false;
    } else {
        _body.clear();
        return true;
    }
}

void RequestProcessor::parseTextPlainUpload(const string &body)
{
    _fileContent = body;

    _filename = "upload_" + cpp11_toString(time(NULL)) + ".txt";

    _fileContentType = "text/plain";

    _formFields["file"] = body;
}

string RequestProcessor::generateContentType()
{
	string toSearch;

	if (_file == nullptr)
		return "text/html";

	map<string, string> mimeTypes;

	mimeTypes[".html"] = "text/html";
	mimeTypes[".htm"] = "text/html";
	mimeTypes[".css"] = "text/css";
	mimeTypes[".js"] = "application/javascript";
	mimeTypes[".json"] = "application/json";
	mimeTypes[".xml"] = "application/xml";
	mimeTypes[".txt"] = "text/plain";
	mimeTypes[".jpg"] = "image/jpeg";
	mimeTypes[".jpeg"] = "image/jpeg";
	mimeTypes[".png"] = "image/png";
	mimeTypes[".gif"] = "image/gif";
	mimeTypes[".bmp"] = "image/bmp";
	mimeTypes[".webp"] = "image/webp";
	mimeTypes[".svg"] = "image/svg+xml";
	mimeTypes[".mp4"] = "video/mp4";
	mimeTypes[".mpeg"] = "video/mpeg";
	mimeTypes[".webm"] = "video/webm";
	mimeTypes[".mov"] = "video/quicktime";
	mimeTypes[".pdf"] = "application/pdf";
	mimeTypes[".zip"] = "application/zip";
	mimeTypes[".tar"] = "application/x-tar";
	mimeTypes[".gz"] = "application/x-gzip";
	mimeTypes[".mp3"] = "audio/mpeg";
	mimeTypes[".wav"] = "audio/wav";
	mimeTypes[".ogg"] = "audio/ogg";
	mimeTypes[".flac"] = "audio/flac";

	size_t pos = _file->getPath().find_last_of('.');
	if (pos != string::npos) {
		string ext = _file->getPath().substr(pos);
		map<string, string>::const_iterator it = mimeTypes.find(ext);
		if (it != mimeTypes.end()) {
			return it->second;
		}
	}

	return "application/octet-stream";

}

string RequestProcessor::getExtensionFromContentType(const string& contentType) const
{
    if (contentType.empty())
        return "bin";

    if (contentType == "image/jpeg" || contentType == "image/jpg")
        return "jpg";
    if (contentType == "image/png")
        return "png";
    if (contentType == "image/gif")
        return "gif";
    if (contentType == "image/bmp")
        return "bmp";
    if (contentType == "image/webp")
        return "webp";
    if (contentType == "image/svg+xml")
        return "svg";

    if (contentType == "video/mp4")
        return "mp4";
    if (contentType == "video/mpeg")
        return "mpeg";
    if (contentType == "video/webm")
        return "webm";
    if (contentType == "video/quicktime")
        return "mov";

    if (contentType == "application/pdf")
        return "pdf";
    if (contentType == "application/zip")
        return "zip";
    if (contentType == "application/x-tar")
        return "tar";
    if (contentType == "application/x-gzip")
        return "gz";
    return "text/html"; 
}

std::ostream &operator<<(std::ostream &os, const RequestProcessor &req)
{
	os << "Request: " << req.getRequest() << endl;
	os << "Method: " << req.getMethod() << endl;
	os << "Uri: " << req.getUri() << endl;
	os << "Host: " << req.getHost() << endl;
	os << "Connection: " << req.getConnection() << endl;
	os << "Content-Length: " << req.getContentLength() << endl;
	os << "Body: " << req.getBody() << endl;
	os << "Query: " << req.getQuery() << endl;
	os << "Cookie: " << req.getCookie() << endl;
	return os;
}

string RequestProcessor::getStoreFileName() const
{
    if (_filename.empty()) {
        return "upload_" + cpp11_toString(time(NULL)) + "." + 
               getExtensionFromContentType(_fileContentType);
    }
    return _filename;
}

string RequestProcessor::getFileContent() const
{
	return _fileContent;
}

map<string, string> RequestProcessor::getFormFields() const
{
	return _formFields;
}

void RequestProcessor::clear() {
    _uri.clear();
    _body.clear();
    _query.clear();
    _cookie.clear();
    _filename.clear();
    _fileContent.clear();
    _fileContentType.clear();
    _headers_parsed = false;
    _responseToSend.clear();
    _responseToSend.clear();
    _connection.clear();
    _content_length = 0;
    _formFields.clear();
    _responded = false;
    _received = false;
    _request.clear();
    _method.clear();
    _body_size = 0;
    _file = NULL;
    _status = 200;
	_client_socket = -1;
}

void    RequestProcessor::init_dangerousePatterns() {
    dangerousPatterns.push_back("..");
    dangerousPatterns.push_back("/./");
    dangerousPatterns.push_back("%2e");
    dangerousPatterns.push_back("%2e.");
    dangerousPatterns.push_back(".%2e");
    dangerousPatterns.push_back("%2e%2e");

    dangerousPatterns.push_back("%2f");
    dangerousPatterns.push_back("%2F");
    dangerousPatterns.push_back("%5c");
    dangerousPatterns.push_back("%5C");

    dangerousPatterns.push_back("%00"); dangerousPatterns.push_back("%01");
    dangerousPatterns.push_back("%02"); dangerousPatterns.push_back("%03");
    dangerousPatterns.push_back("%04"); dangerousPatterns.push_back("%05");
    dangerousPatterns.push_back("%06"); dangerousPatterns.push_back("%07");
    dangerousPatterns.push_back("%08"); dangerousPatterns.push_back("%09");
    dangerousPatterns.push_back("%0a"); dangerousPatterns.push_back("%0b");
    dangerousPatterns.push_back("%0c"); dangerousPatterns.push_back("%0d");
    dangerousPatterns.push_back("%0e"); dangerousPatterns.push_back("%0f");

    dangerousPatterns.push_back("%10"); dangerousPatterns.push_back("%11");
    dangerousPatterns.push_back("%12"); dangerousPatterns.push_back("%13");
    dangerousPatterns.push_back("%14"); dangerousPatterns.push_back("%15");
    dangerousPatterns.push_back("%16"); dangerousPatterns.push_back("%17");
    dangerousPatterns.push_back("%18"); dangerousPatterns.push_back("%19");
    dangerousPatterns.push_back("%1a"); dangerousPatterns.push_back("%1b");
    dangerousPatterns.push_back("%1c"); dangerousPatterns.push_back("%1d");
    dangerousPatterns.push_back("%1e"); dangerousPatterns.push_back("%1f");

    dangerousPatterns.push_back("%22"); 
    dangerousPatterns.push_back("%27"); 
    dangerousPatterns.push_back("%3c"); dangerousPatterns.push_back("%3C"); 
    dangerousPatterns.push_back("%3e"); dangerousPatterns.push_back("%3E"); 
    dangerousPatterns.push_back("%3b"); dangerousPatterns.push_back("%3B"); 
    dangerousPatterns.push_back("%26"); 
    dangerousPatterns.push_back("%7c"); dangerousPatterns.push_back("%7C"); 
}

bool RequestProcessor::isUriBad(std::string uri) {
    for (size_t i = 0; i < dangerousPatterns.size(); i++) {
        if (uri.find(dangerousPatterns[i]) != std::string::npos) {
            std::cout << "⚠️ Dangerous pattern found in URI: " << dangerousPatterns[i] << std::endl;
            return true;
        }
    }
    return false;
}

string RequestProcessor::RedirectionPage(string redirectionUrl, int status_code) {
    string response = "HTTP/1.1 " + cpp11_toString(status_code) + getStatusMessage(status_code);
    response += "Server: webserv/1.0.0 (Ubuntu)\r\n";
    response += "Content-Length: 0\r\n";
    response += "Connection: " + this->getConnection() + "\r\n";
    response += "Location: " + redirectionUrl + "\r\n";
    response += "\r\n";
    return response;
}

bool    RequestProcessor::cgiInUri() {
    Route *route = _server->getRouteFromUri(getUri());
    _route = route;
    if (!route) {
        return false;
    }
    _route = route;
    vector<pair<string, string> > cgiMethods = route->getCGIs();
    for (size_t i = 0; i < cgiMethods.size(); i++) {
        if (this->getUri().find(cgiMethods[i].first) != std::string::npos) {
            _cgi = new CGI();
            _cgi->setUri(this->getUri());
            string path = WORKIN_PATH + _server->getRoot() + cgiMethods[i].second;
            _cgi->setCgiPath(path);
            return true;
        }
    }
    return false;
}

string RequestProcessor::handleCgi(void) {
    if (CURRENT_RUNNING_CGI == MAX_CGI_CALLS) {
        std::cerr << "CGI timeout" << std::endl;
        _status = INTERNAL_SERVER_ERROR_STATUS_CODE;
        return ReturnServerErrorPage(_server, _status);
    }
    string response;

    std::vector<std::string> envVars;
    envVars.push_back("REQUEST_METHOD=" + this->getMethod());
    envVars.push_back("REQUEST_URI=" + this->getUri());
    envVars.push_back("CONTENT_TYPE=" + this->getContentType());
    envVars.push_back("CONTENT_LENGTH=" + cpp11_toString(this->getContentLength()));
    envVars.push_back("HTTP_HOST=" + this->getHost());
    envVars.push_back("HTTP_CONNECTION=" + this->getConnection());
    envVars.push_back("HTTP_COOKIE=" + this->getCookie());
    envVars.push_back("HTTP_AUTHORIZATION=" + this->getAuthorization());
    envVars.push_back("QUERY_STRING=" + this->getQuery());
    envVars.push_back("SERVER_PROTOCOL=HTTP/1.1");
    envVars.push_back("SERVER_SOFTWARE=webserv/1.0.0");
    envVars.push_back("PATH_INFO=" + this->getUri());

    _cgi->setBodyData(this->getBody());

    std::vector<char*> envp;
    for (size_t i = 0; i < envVars.size(); ++i)
        envp.push_back(const_cast<char*>(envVars[i].c_str()));
    envp.push_back(NULL);
    _cgi->setEnvVars(envp);

    if (!_cgi->openInputStream()) {
        std::cerr << "Failed to open input stream for CGI" << std::endl;
        _status = INTERNAL_SERVER_ERROR_STATUS_CODE;
        return ReturnServerErrorPage(_server, _status);
    }

    if (!_cgi->openOutputStream()) {
        std::cerr << "Failed to open output stream for CGI" << std::endl;
        _status = INTERNAL_SERVER_ERROR_STATUS_CODE;
        return ReturnServerErrorPage(_server, INTERNAL_SERVER_ERROR_STATUS_CODE);
    }
    CURRENT_RUNNING_CGI++;
    if (_cgi->execute()) {
        string cgiResponse = _cgi->getCgiOutput();
        _status = OK_STATUS_CODE;
        response = cgiResponse;
    } else {
        std::cerr << "CGI execution failed" << std::endl;
        _status = INTERNAL_SERVER_ERROR_STATUS_CODE;
        response = ReturnServerErrorPage(_server, INTERNAL_SERVER_ERROR_STATUS_CODE);
    }
    CURRENT_RUNNING_CGI--;
    _cgi->clean();
    delete _cgi;
    _cgi = NULL;
    return response;
}

string RequestProcessor::GenerateCostumeErrorPage(int status_code, string error_message) {
    string newBody = "<html><body><h1>" + cpp11_toString(status_code) + " " + error_message + "</h1></body></html>";
    string response = "HTTP/1.1 " + cpp11_toString(status_code) + getStatusMessage(status_code);
    response += "Server: webserv/1.0.0 (Ubuntu)\r\n";
    response += "Content-Type: text/html\r\n";
    response += "Content-Length: " + cpp11_toString(newBody.length()) + "\r\n";
    response += "Connection: close\r\n";
    response += "\r\n";
    response += newBody;
    return response;
}

string RequestProcessor::createResponse(void) {
    string response;
	if (_status != 200) {
		response = this->GenerateCostumeErrorPage(_status, this->getStatusMessage(_status));
		return response;
	}
    if (cgiInUri())
        return this->handleCgi();
    if (isUriBad(this->getUri())) {
        _status = FORBIDDEN_STATUS_CODE;
        response = this->ReturnServerErrorPage(_server, FORBIDDEN_STATUS_CODE);
        return response;
    }
    Route *route = _server->getRouteFromUri(getUri());
    if (!route) {
        return this->ReturnServerErrorPage(_server, NOT_FOUND_STATUS_CODE);
    } else {
        _route = route;
	}
    if (this->getMethod() == "GET") {

		Server *Server = this->_server;
		if (Server == nullptr) {
			cerr << "Error: Server not found" << endl;
			return "";
		}
        if (_route->getRouteGETMethod() == false)
        {
            _status = NOT_ALLOWED_STATUS_CODE;
            response = this->GenerateCostumeErrorPage(NOT_ALLOWED_STATUS_CODE, "GET method not allowed for this route");
            return response;
        }
        if (Server->getRedirectUrl().second != -1) {
            response = this->RedirectionPage(Server->getRedirectUrl().first, Server->getRedirectUrl().second);
            return response;
        }

        File *file = this->GETResponse(Server->getRoot(), this->getUri());
        if (file == nullptr) {
            _status = NOT_FOUND_STATUS_CODE;
            response = this->ReturnServerErrorPage(Server, NOT_FOUND_STATUS_CODE);
        } else {
            this->_file = file;
            response = this->generateHttpHeaders(Server, _status, file->getSize());

        }
	} else if (this->getMethod() == "POST") {
        if (this->getContentLength() > _route->getClientMaxBodySize()) {
            _status = REQUEST_ENTITY_TOO_LARGE_STATUS_CODE;
            response = this->GenerateCostumeErrorPage(REQUEST_ENTITY_TOO_LARGE_STATUS_CODE, "POST request has exceeded max body size");
            return response;
        }
        if (_route->getRoutePOSTMethod() == false)
        {
            _status = NOT_ALLOWED_STATUS_CODE;
            response = this->GenerateCostumeErrorPage(NOT_ALLOWED_STATUS_CODE, "POST method not allowed for this route");
            return response;
        }

        if (_body_size >= _content_length) {
            response = generateHttpHeaders(_server, 200, 0);
        } else {
            _status = INTERNAL_SERVER_ERROR_STATUS_CODE;
            response = this->ReturnServerErrorPage(_server, INTERNAL_SERVER_ERROR_STATUS_CODE);
            return response;
        }
	}
    else if (this->getMethod() == "DELETE") {
        if (_route->getDELETEMethod() == false)
        {
            _status = NOT_ALLOWED_STATUS_CODE;
            response = this->ReturnServerErrorPage(_server, NOT_ALLOWED_STATUS_CODE);
            return response;
        }
        Server *Server = this->_server;
        if (Server == nullptr) {
            cerr << "Error: Server not found" << endl;
            return "";
        }
        response = this->DELETEResponse(Server->getRoot(), this->getUri());
        if (response.empty()) {
            response = this->ReturnServerErrorPage(Server, 404);
        }
    }
    return response;
}

int    RequestProcessor::sendResponse(void)
{
    if (_client_socket == -1) {
        _responded = true;
        return (-1);
    }

    if (_responseToSend.empty())
    {
        _responseToSend = this->createResponse();

        int bytesSent = send(_client_socket, _responseToSend.c_str(), _responseToSend.length(), MSG_NOSIGNAL);
		if (bytesSent == 0) {
			close(_client_socket);
			_client_socket = -1;
			_responded = true;

			return (-1);
		} else if (bytesSent == -1) {
			perror("send");
			_connection = "close";
			_status = TOO_MANY_REQUESTS_STATUS_CODE;
			_responded = true;
			return (-1);
		}
    }

    if (_file)
    {
        if (this->fd == -1)
            this->fd = open(_file->getPath().c_str(), O_RDONLY);
        if (this->fd == -1) {
            perror("open");
            return (-1);
        }
        char buffer[REQUEST_BUFFER_SIZE];
        ssize_t bytesRead = read(this->fd, buffer, REQUEST_BUFFER_SIZE - 1);
        if (bytesRead == -1) {
            perror("read");
            close(this->fd);
            this->fd = -1;
            _responded = true;
            return (-1);
        }
        if ((size_t)bytesRead == 0) {
            close(this->fd);
            this->fd = -1;
            _responded = true;
            return (0);
        }
        buffer[bytesRead] = '\0';
		int bytesSent = send(_client_socket, buffer, bytesRead, MSG_NOSIGNAL);
		if (bytesSent == 0) {
			close(_client_socket);
			_client_socket = -1;
			_responded = true;
			return (-1);
		} else if (bytesSent == -1) {
        }
    } else {
        _responded = true;
    }
    return (1);
}

int	RequestProcessor::receiveRequest(int client_socket) {
    int status;
    char buffer[REQUEST_BUFFER_SIZE] = {0};
    this->_client_socket = client_socket;
	int bytesReceived = recv(client_socket, buffer, REQUEST_BUFFER_SIZE - 1, 0);
	if (bytesReceived > 0) {
		buffer[bytesReceived] = '\0';
		_request.append(buffer, bytesReceived);
		_body_size += bytesReceived;
		if (string(buffer, bytesReceived).find("\r\n\r\n") != \
			string::npos && _headers_parsed == false) {
			_body_size -= (_request.find("\r\n\r\n") + 4);
		}
	} else if (bytesReceived == 0) {
		_client_socket = -1;
		_connection = "close";
		_received = true;
		return true;
	} else {
		return false;
	}
    if (!_headers_parsed && _request.find("\r\n\r\n") != string::npos) {
        status = this->parseHeaders(_request);
        if (status == OK_STATUS_CODE) {
            _headers_parsed = true;
        } else {
            _status = status;
            _received = true;
            cerr << "Error parsing request" << endl;
            return status;
        }
    }
    if (_headers_parsed) {
        if (this->getMethod() == "POST" && _body_size < _content_length) {
            return -1;
        }
        this->parseBody(_request);
        _received = true;
        return 0;
    }
    return status;
}

int RequestProcessor::getSocket() const
{
    return _client_socket;
}

static void print_time(void)
{
    time_t now = time(0);
    tm *ltm = localtime(&now);
    cout
         << ltm->tm_mday << "-"
         << (((ltm->tm_mon + 1) / 10 == 0) ? "0" : "") << (ltm->tm_mon + 1) << "-"
         << ((ltm->tm_year + 1900 / 10 == 0) ? "0" : "") << ltm->tm_year + 1900 << " "
         << ((ltm->tm_hour / 10 == 0) ? "0" : "") << ltm->tm_hour << ":"
         << ((ltm->tm_min / 10 == 0) ? "0" : "") << ltm->tm_min << ":"
         << ((ltm->tm_sec / 10 == 0) ? "0" : "") << ltm->tm_sec;
}

void RequestProcessor::log() const {
    cout << def << "[";
    print_time();
    cout << "] ";
    if (_host.empty()) {
        cout << bold << "CONNECTION CLOSED" << def << endl;
        return;
    }
    if (_method == "POST")
        cout << bold << blue;
    else if (_method == "GET")
        cout << bold << green;
    else
        cout << bold << red;
    cout << _method << def << " " << _uri << " " << _host << ":" << _port << " ";
    if (_status == OK_STATUS_CODE)
        cout << bold << green;
    else
        cout << bold << red;
    cout << _status << def << endl;
}

void		RequestProcessor::setPort(string new_port) {
    _port = new_port;
}