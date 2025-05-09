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

vector<string> split(string str, char delimiter);
string trim(const string &str);
vector<string> splitByString(string str, string delimiter);

RequestProcessor::RequestProcessor() {
    _headers_parsed = false;
    _content_length = 0;
    _body_size = 0;
    _fully_sent = false;
    _file = nullptr;
    fd = -1;
}

RequestProcessor::~RequestProcessor() {}

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

int RequestProcessor::getContentLength() const
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

void    RequestProcessor::setIsSent(bool sent)
{
    _fully_sent = sent;
}

bool RequestProcessor::isSent() const
{
    return _fully_sent;
}

void RequestProcessor::setResponseToSend(string response)
{
    _responseToSend = response;
}

string RequestProcessor::getResponseToSend() const
{
    return _responseToSend;
}

string RequestProcessor::generateHttpHeaders(Server *server, int status_code, long fileSize)
{
    string date;
    /*generate Date http*/
    time_t now = time(0);
    struct tm* gmt = gmtime(&now);
    char date_buffer[128];
    strftime(date_buffer, sizeof(date_buffer), "%a, %d %b %Y %H:%M:%S GMT", gmt);
    date = "Date: " + string(date_buffer) + "\r\n";
    string _Http_headers;
    (void)server;
    _Http_headers = "HTTP/1.1 " + cpp11_toString(status_code) + getStatusMessage(status_code);
    _Http_headers += "Server: webserv/1.0.0 (Ubuntu)\r\n";
    _Http_headers += date;
    _Http_headers += "Content-Type: " + (status_code >= 200 && status_code < 300 ? generateContentType() : "text/html") + "\r\n";
    _Http_headers += "Content-Length: " + cpp11_toString(fileSize) + "\r\n";
    _Http_headers += "Connection: " + _connection + "\r\n";
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
	cout << "In Directory Listing" << endl;
    string htmlContent = "<html><head><title>Directory Listing</title></head><body><h1>Directory Listing</h1><ul>";
    DIR* dir = opendir(path.c_str());
    string routeName = _route->getRouteName();
    routeName = cpp11_replace(routeName, "\"", "");
    if (dir != NULL) {
        struct dirent* entry;
        while ((entry = readdir(dir)) != NULL) {
            string name = entry->d_name;
            if (name != "." && name != "..") {
                htmlContent += "<li><a href=\"";
                htmlContent += routeName + "/" + name;
                htmlContent += "\">";
                htmlContent += routeName + "/" + name;
                htmlContent += "</a></li>";
            }
        }
        closedir(dir);
    }
    htmlContent += "</ul></body></html>";
    return htmlContent;
}

File* RequestProcessor::handleDirectory(const string& path) const {
    vector<string> indexFiles = _route->getRouteIndexFiles();
    if (!indexFiles.empty())
    {
        string indexPath = processIndexFiles(indexFiles);
        if (!indexPath.empty()) {
            cout << "Found index file: " << indexPath << endl;
            return GetFile(indexPath);
        }
    }

    if (!this->_route->getRouteDirectoryListing())
        return NULL;

    string htmlContent = createDirectoryListing(path);
    string tempPath = "/tmp/dirlist_" + cpp11_toString(time(NULL)) + ".html";
    ofstream tempFile(tempPath.c_str());
    if (!tempFile.is_open())
        return NULL;
    
    tempFile << htmlContent;
    tempFile.close();
    return new File(tempPath);
}

string     RequestProcessor::ReturnServerErrorPage(Server *server, int status_code)//should be a global function
{
    /* File    error_page(server->getErrorPage());// till we have the server */(void)server;
    File    error_page("./error/404.html");
    std::string response = generateHttpHeaders(server, status_code, error_page.getSize());
    response += error_page.getData();
    return response;
}

string RequestProcessor::processIndexFiles(vector<string> &indexFiles) const {
    for (size_t i = 0; i < indexFiles.size(); i++) {
        string indexPath = "/tmp/www/" + indexFiles[i];
        cout << "Checking index path: " << indexPath << endl;
        if (access(indexPath.c_str(), F_OK) == 0) {
            return indexPath;
        }
    }
    return "";
}

File* RequestProcessor::GETResponse(string root, string requestedPath) const {
    string routeName = _route->getRouteName();
    string reqUri = this->getUri();
    string basePath = "/tmp/www/" + root + reqUri.substr(1);
    if (requestedPath == "/") {
        vector<string> indexFiles = _route->getRouteIndexFiles();

        basePath = processIndexFiles(indexFiles);
        if (basePath.empty() ) {//ghir ila dirlisting == on
            return handleDirectory(basePath);
        }
    }
    struct stat pathStat;
    if (stat(basePath.c_str(), &pathStat) == -1) {
        return NULL;
    }
    if (S_ISREG(pathStat.st_mode)) {
        return GetFile(basePath);
    }
    if (S_ISDIR(pathStat.st_mode)) {
        return handleDirectory(basePath);
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
			
			// URL decode the key and value if needed
			_formFields[key] = value;
		}
	}
}

RequestProcessor::RequestProcessor(string req, string __port, Server *server)
{
    _request = req;
    _server = server;
    _port = __port;
    _fully_sent = false;
    
    if (this->parseHeaders(req))
        cerr << "Error parsing headers" << endl;
    if (this->parseBody(req))
        cerr << "Error parsing body" << endl;
}

int    RequestProcessor::parseHeaders(string req) {
    size_t headerEnd = req.find("\r\n\r\n");
    if (headerEnd == string::npos) {
        headerEnd = req.find("\n\n"); // Fallback for non-standard requests
        if (headerEnd == string::npos) {
            cerr << "Invalid request format: couldn't find header/body separator" << endl;
            return (1);
        }
    }
    
    // Extract headers section and body
    string headersSection = req.substr(0, headerEnd);
    
    // Normalize line endings in headers section and split into lines
    // Replace all \r\n with \n for consistent processing
    string normalizedHeaders = headersSection;
    size_t pos = 0;
    while ((pos = normalizedHeaders.find("\r\n", pos)) != string::npos) {
        normalizedHeaders.replace(pos, 2, "\n");
        pos++;
    }
    
    // Split headers by \n
    vector<string> headers = split(normalizedHeaders, '\n');
    if (headers.empty()) {
        cerr << "Invalid request: no headers found" << endl;
        return (1);
    }
    
    // Process the first line (request line)
    vector<string> requestLine = splitByString(headers[0], " ");
    if (requestLine.size() < 2) {
        cerr << "Invalid request line format" << endl;
        return (1);
    }
    
    _method = requestLine[0];
    if (_method != "GET" && _method != "POST") {
        cerr << "Unsupported HTTP method: " << _method << endl;
        return (1);
    }
    _uri = requestLine[1];
    
    // Extract query parameters if present
    size_t queryPos = _uri.find('?');
    if (queryPos != string::npos) {
        _query = _uri.substr(queryPos + 1);
        _uri = _uri.substr(0, queryPos);
    }
    
    // Process the rest of the headers
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
                _content_length = std::atoi(value.c_str());
            } else if (key == "Connection") {
                _connection = value;
            } else if (key == "Cookie") {
                _cookie = value;
            }
        }
    }
    
    return (0);
}

int RequestProcessor::parseBody(string req) {
    if (_method == "POST") {
        size_t headerEnd = req.find("\r\n\r\n");
        if (headerEnd == string::npos) {
            headerEnd = req.find("\n\n"); // Fallback for non-standard requests
            if (headerEnd == string::npos) {
                cerr << "Invalid request format: couldn't find header/body separator" << endl;
                return (1);
            }
        }
        
        // Extract headers section and body
        string headersSection = req.substr(0, headerEnd);
        
        // Get the body (everything after the empty line)
        _body = "";
        if (headerEnd + 4 < req.length()) {
            _body = req.substr(headerEnd + 4); // Skip "\r\n\r\n"
        } else if (headerEnd + 2 < req.length()) {
            _body = req.substr(headerEnd + 2); // Skip "\n\n" for non-standard
        }

        if (_content_type.find("application/x-www-form-urlencoded") != string::npos) {
            parseFormUrlEncoded(_body);
        } else if (_content_type.find("multipart/form-data") != string::npos) {
            size_t boundaryPos = _content_type.find("boundary=");
            if (boundaryPos != string::npos) {
                string boundary = _content_type.substr(boundaryPos + 9);
                
                // Remove quotes if present
                if (boundary[0] == '"' && boundary[boundary.length()-1] == '"') {
                    boundary = boundary.substr(1, boundary.length() - 2);
                }
                parseMultipartFormData(_body, boundary);
            }
        }
        else if (_content_type.find("text/plain") != string::npos)
            parseTextPlainUpload(_body);
        else if (_content_type.find("application/json") != string::npos) {
            _fileContent = _body;
            _filename = "upload_" + cpp11_toString(time(NULL)) + ".json";
            _fileContentType = "application/json";
        }
        else {
            cerr << "Unsupported Content-Type: " << _content_type << endl;
            return (1);
        }
    }
    return (0);
}

string RequestProcessor::getFileContentType() const
{
    return _fileContentType;
}

void RequestProcessor::parseMultipartFormData(const string &body, const string &boundary)
{
    // The boundary in the Content-Type header doesn't include the leading "--"
    // but in the request body, each boundary starts with "--"
    string boundaryDelimiter = "--" + boundary;
    string endBoundary = boundaryDelimiter + "--";
    
    // cout << "Searching with boundary: '" << boundaryDelimiter << "'" << endl;
    
    size_t pos = 0;
    size_t nextBoundaryPos;
    
    while ((nextBoundaryPos = body.find(boundaryDelimiter, pos)) != string::npos) {
        // Skip past the current boundary line
        size_t partStart = body.find("\r\n", nextBoundaryPos);
        if (partStart == string::npos) break;
        partStart += 2; // Skip "\r\n"
        
        // Find the next boundary
        size_t partEnd = body.find(boundaryDelimiter, partStart);
        if (partEnd == string::npos) break;
        // Extract the part content (includes headers and data)
        string part = body.substr(partStart, partEnd - partStart);
        
        // Split headers from content
        size_t headerEnd = part.find("\r\n\r\n");
        if (headerEnd == string::npos) continue;
        
        string headers = part.substr(0, headerEnd);
        string content = part.substr(headerEnd + 4); // Skip "\r\n\r\n"
        
        // Remove trailing CRLF if present
        if (content.length() >= 2 && content.substr(content.length() - 2) == "\r\n") {
            content = content.substr(0, content.length() - 2);
        }
        
        // Parse Content-Disposition to get field name and filename
        size_t namePos = headers.find("name=\"");
        size_t filenamePos = headers.find("filename=\"");
        
        string fieldName;
        if (namePos != string::npos) {
            namePos += 6; // Skip 'name="'
            size_t nameEnd = headers.find("\"", namePos);
            if (nameEnd != string::npos)
                fieldName = headers.substr(namePos, nameEnd - namePos);
        }
        
        // Store form field value
        if (!fieldName.empty()) {
            _formFields[fieldName] = content;
            // cout << "Found field: " << fieldName << " with length: " << content.length() << endl;
        }
        
        // Handle file upload
        if (filenamePos != string::npos) {
            filenamePos += 10; // Skip 'filename="'
            size_t filenameEnd = headers.find("\"", filenamePos);
            if (filenameEnd != string::npos) {
                _filename = headers.substr(filenamePos, filenameEnd - filenamePos);
                _fileContent = content;
                
                // Get content type
                size_t contentTypePos = headers.find("Content-Type:");
                if (contentTypePos != string::npos) {
                    contentTypePos += 13; // Skip "Content-Type:"
                    size_t ctEnd = headers.find("\r\n", contentTypePos);
                    if (ctEnd != string::npos)
                        _fileContentType = trim(headers.substr(contentTypePos, ctEnd - contentTypePos));
                    else
                        _fileContentType = trim(headers.substr(contentTypePos));
                }
            }
        }
        pos = partEnd;
    }
}

void RequestProcessor::parseTextPlainUpload(const string &body)
{
    // For text/plain uploads, store the entire body as file content
    _fileContent = body;
    
    // Set a default filename with timestamp
    _filename = "upload_" + cpp11_toString(time(NULL)) + ".txt";
    
    // Set content type
    _fileContentType = "text/plain";
    
    // Also add to form fields for consistency
    _formFields["file"] = body;
}

string RequestProcessor::generateContentType()
{
    if (_uri == "/") {
        return "text/html";
    }
    if (_uri.find(".html") != string::npos || _uri.find(".htm") != string::npos) {
        return "text/html";
    }
    if (_uri.find(".css") != string::npos) {
        return "text/css";
    }
    if (_uri.find(".js") != string::npos) {
        return "application/javascript";
    }
    if (_uri.find(".json") != string::npos) {
        return "application/json";
    }
    if (_uri.find(".xml") != string::npos) {
        return "application/xml";
    }
    if (_uri.find(".txt") != string::npos) {
        return "text/plain";
    }
    if (_uri.find(".jpg") != string::npos || _uri.find(".jpeg") != string::npos) {
        return "image/jpeg";
    }
    if (_uri.find(".png") != string::npos) {
        return "image/png";
    }
    if (_uri.find(".gif") != string::npos) {
        return "image/gif";
    }
    if (_uri.find(".bmp") != string::npos) {
        return "image/bmp";
    }
    if (_uri.find(".webp") != string::npos) {
        return "image/webp";
    }
    if (_uri.find(".svg") != string::npos) {
        return "image/svg+xml";
    }
    if (_uri.find(".mp4") != string::npos) {
        return "video/mp4";
    }
    if (_uri.find(".mpeg") != string::npos) {
        return "video/mpeg";
    }
    if (_uri.find(".webm") != string::npos) {
        return "video/webm";
    }
    if (_uri.find(".mov") != string::npos) {
        return "video/quicktime";
    }
    if (_uri.find(".pdf") != string::npos) {
        return "application/pdf";
    }
    if (_uri.find(".zip") != string::npos) {
        return "application/zip";
    }
    if (_uri.find(".tar") != string::npos) {
        return "application/x-tar";
    }
    if (_uri.find(".gz") != string::npos) {
        return "application/x-gzip";
    }
    if (_uri.find(".mp3") != string::npos) {
        return "audio/mpeg";
    }
    if (_uri.find(".wav") != string::npos) {
        return "audio/wav";
    }
    if (_uri.find(".ogg") != string::npos) {
        return "audio/ogg";
    }
    if (_uri.find(".flac") != string::npos) {
        return "audio/flac";
    }
    return "text/html";
}


string RequestProcessor::getExtensionFromContentType(const string& contentType) const
{
    // Default extension is bin for unknown types
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
        // Generate a default name with appropriate extension
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
    _request.clear();
    _method.clear();
    _uri.clear();
    _connection.clear();
    _content_length = 0;
    _body.clear();
    _query.clear();
    _cookie.clear();
    _filename.clear();
    _fileContent.clear();
    _fileContentType.clear();
    _formFields.clear();
    _headers_parsed = false;
    _body_size = 0;
}

string RequestProcessor::createResponse(void) {
    string response;
    if (this->getMethod() == "GET") {
		Server *Server = this->_server;
		if (Server == nullptr) {
			cerr << "Error: Server not found" << endl;
			return "";
		}
		Route *route = Server->getRouteFromUri(this->getUri());
		if (route == nullptr) {
			response = this->ReturnServerErrorPage(Server, 404);
			cout << "Route not found" << endl;
		} else {
            this->_route = route;
            File *file = this->GETResponse(Server->getRoot(), this->getUri());
            if (file == nullptr) {
                response = this->ReturnServerErrorPage(Server, 404);
                cout << "File not found" << endl;
            } else {
                this->_file = file;
                response = this->generateHttpHeaders(Server, 200, file->getSize());
            }
        }
	} else if (this->getMethod() == "POST") {
        Server *Server = this->_server;
		if (Server == nullptr) {
			cerr << "Error: Server not found" << endl;
			return "";//500 internal server error
		}
		Route *route = Server->getRouteFromUri(this->getUri());
        if (route == nullptr) {
			response = this->ReturnServerErrorPage(Server, 404);
			cout << "Route not found hmmmm" << endl;
		}
        else
        {
            cerr << "Route found: " << route->getRouteName() << " and uri is: " << this->getUri() << endl;
            cerr << red << "Server is " << *Server << def << endl;
            this->_route = route;
            _fully_sent = true;
            response = POSTResponse();
        }
	}
    else {
        cerr << "Unsupported method: " << _method << endl;
        return "";
    }
    return response;
}

string  RequestProcessor::POSTResponse()
{

    cout << "In POSTResponse" << endl;

    string response;
    string initUploadPath = "./body/";
    if (_route->getUploadStore() != "") {
        initUploadPath = "/tmp/www/" + _route->getUploadStore();
    }
    cerr << blue << "Upload path: " << initUploadPath << def << endl;
    if (access(initUploadPath.c_str(), R_OK) == -1) {
        cerr << "Upload path does not exist, creating it" << endl;
        if (mkdir(initUploadPath.c_str(), 0777) == -1) {
            cerr << "Failed to create upload path" << endl;
            return this->ReturnServerErrorPage(_server, 500);
        }
    }
    if (!this->getFileContent().empty()) {
        string uploadPath = initUploadPath + this->getStoreFileName();
        ofstream outFile(uploadPath.c_str(), ios::binary);
        if (outFile.is_open()) {
            outFile.write(this->getFileContent().c_str(), this->getFileContent().length());
            outFile.close();
            response = generateHttpHeaders(_server, 200, 0);
            // response += "<html><body><h1>File uploaded successfully</h1>";hmmm
        } else {
            cerr << "Failed to save uploaded file" << endl;
            response = this->ReturnServerErrorPage(_server, 500);
        }
        send(this->getSocket(), response.c_str(), response.length(), 0);
    } else {
        cout << bold << red << "No file uploaded" << def << endl;
        response = this->ReturnServerErrorPage(_server, 400);
        send(this->getSocket(), response.c_str(), response.length(), 0);
    }


    return (response);
}

int    RequestProcessor::sendResponse(void)
{
    if (_client_socket == -1) {
        cout << "Client socket is closed" << endl;
        return (-1);
    }

    if (_responseToSend.empty()) {

        _responseToSend = this->createResponse();

        cout << "Sending response: " << _responseToSend << endl;
        int bytesSent = send(_client_socket, _responseToSend.c_str(), _responseToSend.length(), 0);
        if (bytesSent == -1) {
            perror("send() failed");
            return (-1);
        }
        cout << "Sent " << bytesSent << " bytes to client" << endl;
        cout << "Response sent: " << _responseToSend << endl;
    }

    if (_file)
    {
        if (this->fd == -1)
            this->fd = open(_file->getPath().c_str(), O_RDONLY);
        if (this->fd == -1) {
            cout << _file->getPath() << " " << endl;
            perror("open() failed");
            return (-1);
        }
        char buffer[REQUEST_BUFFER_SIZE];
        ssize_t bytesRead = read(this->fd, buffer, REQUEST_BUFFER_SIZE - 1);
        cout << "Sent " << bytesRead << " bytes to client" << endl;
        if (bytesRead == -1) {
            perror("read() failed");
            close(this->fd);
            this->fd = -1;
            _fully_sent = true;
            return (-1);
        }
        if ((size_t)bytesRead == 0) {
            cout << "File fully sent" << endl;
            close(this->fd);
            this->fd = -1;
            _fully_sent = true;
            return (0);
        }
        buffer[bytesRead] = '\0';
        send(_client_socket, buffer, bytesRead, 0);
        cout << "Sending file: " << buffer << endl;
    } else {
        cout << "No file to send" << endl;
        _fully_sent = true;
    }
    return (1);
}

bool	RequestProcessor::receiveRequest(int client_socket) {
    char buffer[REQUEST_BUFFER_SIZE] = {0};
    this->_client_socket = client_socket;
    while (true) {
        int bytesReceived = recv(client_socket, buffer, REQUEST_BUFFER_SIZE - 1, 0);
        if (bytesReceived > 0) {
            buffer[bytesReceived] = '\0';
            _request.append(buffer, bytesReceived);
            _body_size += bytesReceived;
            if (string(buffer, bytesReceived).find("\r\n\r\n") != string::npos) {
                _body_size -= (_request.find("\r\n\r\n") + 4);
            }
        } else if (bytesReceived == 0) {
            _client_socket = -1;
            return true;  // client sd connection
        } else if (errno == EAGAIN || errno == EWOULDBLOCK) {
            break ;
        } else {
            perror("recv() failed");
            return false;
        }
    }
    if (!_headers_parsed && _request.find("\r\n\r\n") != string::npos) {
        if (this->parseHeaders(_request) == 0) {
            _headers_parsed = true;
        } else {
            cerr << "Error parsing request" << endl;
            return false;
        }
    }
    if (_headers_parsed) {
        if (this->getMethod() == "GET") {
            return true;
        } else if (this->getMethod() == "POST") {
            if (_body_size >= _content_length) {
                this->parseBody(_request);
                return true;
            }
        } else {
            cerr << "Unsupported method: " << _method << endl;
            return false;
        }
    }
    return false;
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
    if (_client_socket == -1) {
        cout << "DISCONNECTED" << endl;
        return;
    }
    if (_method == "POST")
        cout << bold << blue;
    else if (_method == "GET")
        cout << bold << green;
    else
        cout << bold << red;
    cout << _method << def << " " << _uri << " " << _host << ":" << _port << endl;
}

void		RequestProcessor::setPort(string new_port) {
    _port = new_port;
}
