/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   RequestProccessor.cpp                              :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: zouddach <zouddach@student.1337.ma>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/02/06 16:46:10 by zouddach          #+#    #+#             */
/*   Updated: 2025/04/17 21:01:29 by zouddach         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "RequestProccessor.hpp"

vector<string> split(string str, char delimiter);
string trim(const string &str);
vector<string> splitByString(string str, string delimiter);

RequestProccessor::RequestProccessor() {}

RequestProccessor::~RequestProccessor() {}

string RequestProccessor::getRequest() const
{
	return _request;
}

string RequestProccessor::getMethod() const
{
	return _method;
}

string RequestProccessor::getUri() const
{
	return _uri;
}

string RequestProccessor::getHost() const
{
	return _host;
}

string RequestProccessor::getPort() const
{
	return _port;
}

string RequestProccessor::getConnection() const
{
	return _connection;
}

string RequestProccessor::getContentLength() const
{
	return _content_length;
}

string RequestProccessor::getBody() const
{
	return _body;
}

string RequestProccessor::getQuery() const
{
	return _query;
}

string RequestProccessor::getCookie() const
{
	return _cookie;
}

void RequestProccessor::debugRequest() const
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
        cerr << "File content length: " << _fileContent.length() << " bytes" << endl;
    }
    
    cerr << "Form fields: " << endl;
    for (map<string, string>::const_iterator it = _formFields.begin(); it != _formFields.end(); ++it)
    {
        cerr << "  " << it->first << ": ";
        // Limit output length for large values
        if (it->second.length() > 50)
            cerr << it->second.substr(0, 50) << "... (" << it->second.length() << " bytes)";
        else
            cerr << it->second;
        cerr << endl;
    }
    cerr << "======================" << endl;
}

void RequestProccessor::parseFormUrlEncoded(const string &body)
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

RequestProccessor::RequestProccessor(string req, string __port, Server *server)
{
	_request = req;
	_server = server;

	// Split headers and body using proper HTTP delimiter
	vector<string> request_parts = splitByString(req, "\r\n\r\n");
	if (request_parts.empty())
	{
		throw runtime_error("Invalid request: empty request");
	}

	// Parse headers section
	vector<string> lines = split(request_parts[0], '\n');
	if (lines.empty())
	{
		throw runtime_error("Invalid request: missing request line");
	}

	// Parse first line (request line)
	string first_line = trim(lines[0]);
	vector<string> first_line_parts = split(first_line, ' ');
	if (first_line_parts.size() != 3)
	{ // Must have METHOD URI HTTP/VERSION
		throw runtime_error("Invalid request line: " + first_line);
	}

	// Set method and parse URI
	_method = trim(first_line_parts[0]);
	string uri_str = trim(first_line_parts[1]);

	// Parse URI and query parameters
	size_t query_pos = uri_str.find('?');
	if (query_pos != string::npos)
	{
		_uri = uri_str.substr(0, query_pos);
		_query = uri_str.substr(query_pos + 1);
	}
	else
	{
		_uri = uri_str;
		_query = "";
	}

	// Parse headers
	string content_type;
	for (size_t i = 1; i < lines.size(); i++)
	{
		string line = trim(lines[i]);
		if (line.empty())
			continue;

		size_t colon_pos = line.find(':');
		if (colon_pos == string::npos)
			continue;

		string key = trim(line.substr(0, colon_pos));
		string value = trim(line.substr(colon_pos + 1));

		if (key == "Host")
		{
			size_t port_pos = value.find(':');
			if (port_pos != string::npos)
			{
				_host = trim(value.substr(0, port_pos));
				_port = __port;
			}
			else
			{
				_host = value;
				_port = __port;
			}
			cout << blue << "Port has been set to: " << _port << def << endl;
		}
		else if (key == "Connection")
		{
			_connection = value;
		}
		else if (key == "Content-Length")
		{
			_content_length = value;
		}
		else if (key == "Cookie")
		{
			_cookie = value;
		}
		else if (key == "Content-Type")
		{
			content_type = value;
    		_content_type = value;
		}
	}

	if (request_parts.size() > 1)
	{
		_body = request_parts[1];
		
		if (_method == "POST" && !content_type.empty())
		{
			_content_type = content_type;
			
			if (content_type.find("multipart/form-data") != string::npos)
			{
				// Handle multipart form data (file uploads)
				size_t boundary_pos = content_type.find("boundary=");
				if (boundary_pos != string::npos)
				{
					string boundary = content_type.substr(boundary_pos + 9);
					// Remove quotes if present
					if (boundary[0] == '"' && boundary[boundary.length()-1] == '"')
						boundary = boundary.substr(1, boundary.length() - 2);
					
					cout << "Parsing multipart form data with boundary: " << boundary << endl;
					parseMultipartFormData(_body, boundary);
				}
			}
			else if (content_type.find("application/x-www-form-urlencoded") != string::npos)
			{
				// Handle standard form submission
				parseFormUrlEncoded(_body);
			}
			else if (content_type.find("application/json") != string::npos)
			{
				// For JSON content, keep the body as is and let handler process it
				cout << "JSON content detected: " << _body.substr(0, 30) << "..." << endl;
			}
		}
	}
}

void RequestProccessor::parseMultipartFormData(const string &body, const string &boundary)
{
    string boundaryDelimiter = "--" + boundary;
    string endBoundary = boundaryDelimiter + "--";
    
    size_t pos = 0;
    size_t nextBoundaryPos;
    
    while ((nextBoundaryPos = body.find(boundaryDelimiter, pos)) != string::npos) {
        // Skip past this boundary
        pos = nextBoundaryPos + boundaryDelimiter.length();
        
        // Check if we've hit the end boundary
        if (body.substr(pos, 2) == "--")
            break;
            
        // Skip CRLF after boundary
        if (body.substr(pos, 2) == "\r\n")
            pos += 2;
            
        // Find the end of this part
        size_t nextPartPos = body.find(boundaryDelimiter, pos);
        if (nextPartPos == string::npos)
            break;
            
        // Extract part content including headers
        string part = body.substr(pos, nextPartPos - pos);
        
        // Split headers and content
        size_t headerEnd = part.find("\r\n\r\n");
        if (headerEnd != string::npos) {
            string headers = part.substr(0, headerEnd);
            string content = part.substr(headerEnd + 4); // Skip the double CRLF
            
            // Remove trailing CRLF from content if present
            if (content.length() >= 2 && content.substr(content.length() - 2) == "\r\n") {
                content = content.substr(0, content.length() - 2);
            }
            
            // Parse Content-Disposition header for field name and filename
            size_t namePos = headers.find("name=\"");
            string fieldName;
            string filename;
            
            if (namePos != string::npos) {
                namePos += 6; // Skip 'name="'
                size_t nameEnd = headers.find("\"", namePos);
                if (nameEnd != string::npos) {
                    fieldName = headers.substr(namePos, nameEnd - namePos);
                }
            }
            
            size_t filenamePos = headers.find("filename=\"");
            if (filenamePos != string::npos) {
                filenamePos += 10; // Skip 'filename="'
                size_t filenameEnd = headers.find("\"", filenamePos);
                if (filenameEnd != string::npos) {
                    filename = headers.substr(filenamePos, filenameEnd - filenamePos);
                }
            }
            
            // Store field in the map
            if (!fieldName.empty()) {
                _formFields[fieldName] = content;
                
                // If it's a file, store its info
                if (!filename.empty()) {
                    _filename = filename;
                    _fileContent = content;
                }
            }
        }
        
        // Move position to process next part
        pos = nextPartPos;
    }
}


std::ostream &operator<<(std::ostream &os, const RequestProccessor &req)
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

string RequestProccessor::getStoreFileName() const
{
    if (_filename.empty()) {
        return "upload_" + cpp11_toString(time(NULL)) + ".bin";
    }
    return _filename;
}

string RequestProccessor::getFileContent() const
{
	return _fileContent;
}

map<string, string> RequestProccessor::getFormFields() const
{
	return _formFields;
}