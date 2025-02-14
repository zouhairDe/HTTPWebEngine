/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   RequestProccessor.cpp                              :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: zouddach <zouddach@1337.student.ma>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/02/06 16:46:10 by zouddach          #+#    #+#             */
/*   Updated: 2025/02/14 19:17:05 by zouddach         ###   ########.fr       */
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

RequestProccessor::RequestProccessor(string req)
{
	_request = req;

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
				_port = trim(value.substr(port_pos + 1));
			}
			else
			{
				_host = value;
				_port = "80";
			}
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
		}
	}

	// Handle body and multipart form data
	if (request_parts.size() > 1)
	{
		_body = request_parts[1];

		if (_method == "POST" && !content_type.empty())
		{
			if (content_type.find("multipart/form-data") != string::npos)
			{
				size_t boundary_pos = content_type.find("boundary=");
				if (boundary_pos != string::npos)
				{
					string boundary = content_type.substr(boundary_pos + 9);
					// Remove any quotes around boundary
					if (boundary[0] == '"')
						boundary = boundary.substr(1, boundary.length() - 2);
					// Now we can parse multipart form data using the boundary
					parseMultipartFormData(_body, boundary);
				}
			}
		}
	}
}

void RequestProccessor::parseMultipartFormData(const string &body, const string &boundary)
{
	vector<string> parts = splitByString(body, "--" + boundary);
	for (size_t i = 0; i < parts.size(); i++)
	{
		string part = parts[i];
		if (part.empty() || part == "--")
			continue;

		vector<string> part_lines = split(part, '\n');
		for (size_t i = 0; i < part_lines.size(); i++)
		{
			string line = trim(part_lines[i]);
			line = trim(line);
			if (line.find("Content-Disposition:") != string::npos)
			{
				// Parse filename and field name
				size_t filename_pos = line.find("filename=\"");
				if (filename_pos != string::npos)
				{
					size_t end_pos = line.find("\"", filename_pos + 10);
					if (end_pos != string::npos)
					{
						string filename = line.substr(filename_pos + 10,
													  end_pos - (filename_pos + 10));
						// Store filename for processing
						// _filename = filename;
					}
				}
			}
		}
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