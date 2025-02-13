/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   RequestProccessor.cpp                              :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: zouddach <zouddach@1337.student.ma>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/02/06 16:46:10 by zouddach          #+#    #+#             */
/*   Updated: 2025/02/06 19:07:46 by zouddach         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

# include "RequestProccessor.hpp"

vector<string> split(string str, char delimiter);
string trim(const string& str);
vector<string> splitByString(string str, string delimiter);

RequestProccessor::RequestProccessor() { }

RequestProccessor::~RequestProccessor() { }

string	RequestProccessor::getRequest() const {
	return _request;
}

string	RequestProccessor::getMethod() const {
	return _method;
}

string	RequestProccessor::getUri() const {
	return _uri;
}

string	RequestProccessor::getHost() const {
	return _host;
}

string	RequestProccessor::getConnection() const {
	return _connection;
}

string	RequestProccessor::getContentLength() const {
	return _content_length;
}

string	RequestProccessor::getBody() const {
	return _body;
}

string	RequestProccessor::getQuery() const {
	return _query;
}

string	RequestProccessor::getCookie() const {
	return _cookie;
}

RequestProccessor::RequestProccessor(string req) {
	_request = req;
	vector<string> lines = split(req, '\n');
	if (lines.size() < 1) {
		throw runtime_error("Invalid request 1");
		exit (1);
	}
	//first proccess the first line of the request
	//uri and method
	string first_line = lines[0];
	vector<string> first_line_parts = split(first_line, ' ');
	if (first_line_parts.size() != 3) {
		throw runtime_error("Invalid request 2");
		exit (1);
	}
	_method = first_line_parts[0];
	_uri = first_line_parts[1];//later we gonna need to split it to get the query

	for (size_t i = 1; i < lines.size(); i++) {
		vector<string> parts = split(lines[i], ':');
		if (parts.size() >= 2) {
			string key = parts[0];
			string value = parts[1];
			if (key == "Host") {
				_host = parts[1], _port = parts[2];
			} else if (key == "Connection") {
				_connection = trim(value);
			} else if (key == "Content-Length") {
				_content_length = trim(value);
			} else if (key == "Cookie") {
				_cookie = trim(value);
			}
		}
	}
	
	//getting the body
	vector<string> body_parts = splitByString(_request, "\r\n\r\n");
	if (body_parts.size() == 2) {
		_body = body_parts[1];
	} else {
		_body = "";
	}
	
}