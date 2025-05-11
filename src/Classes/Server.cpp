/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   WebServer.cpp                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: zouddach <zouddach@student.1337.ma>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/02/06 15:32:33 by zouddach          #+#    #+#             */
/*   Updated: 2025/04/17 20:52:03 by zouddach         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Server.hpp"

Server::Server(string hostname, string port, string root)
	: HostName(hostname), Port(port), Root(root), ClientMaxBodySize(1024*1024), Socket(-1) {
		_redirectionUrl = make_pair("", -1);
}

Server::~Server() { }

Server &Server::operator=(const Server &server) {
	if (this != &server) {
		this->HostName = server.HostName;
		this->Port = server.Port;
		this->Root = server.Root;
		this->ClientMaxBodySize = server.ClientMaxBodySize;
		this->Socket = server.Socket;
		this->IndexFiles = server.IndexFiles;
		this->ServerNames = server.ServerNames;
		this->ErrorPage404 = server.ErrorPage404;
		this->ErrorPage403 = server.ErrorPage403;
		this->ErrorPage500 = server.ErrorPage500;
		this->_Routes = server._Routes;
		this->_ServerFriends = server._ServerFriends;
		this->_redirectionUrl = server._redirectionUrl;
	}
	return (*this);
}

int Server::init(int epoll_fd) {
	int opt;
	struct sockaddr_in server_addr;
	struct epoll_event new_event;

	this->Socket = socket(AF_INET, SOCK_STREAM, 0);
	if (this->Socket == -1) {
		cerr << "Error creating socket" << endl;
		return (1);
	}
	opt = 1;
	if (setsockopt(this->Socket, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) == -1) {
		cerr << "Error setting socket options" << endl;
		return (1);
	}

	cout << "port: " << this->getPort() << endl;
	server_addr.sin_family = AF_INET;
	server_addr.sin_addr.s_addr = INADDR_ANY;
	int port = atoi(this->getPort().c_str());
	server_addr.sin_port = htons(port);
	if (bind(this->Socket, (struct sockaddr *)&server_addr, sizeof(server_addr)) == -1) {
		cerr << "Error binding socket" << endl;
		return (1);
	}
	if (listen(this->Socket, SOMAXCONN) == -1) {
		cerr << "Error listening on socket" << endl;
		return (1);
	}
	if (fcntl(Socket, F_SETFD, FD_CLOEXEC) == -1)
	{
		std::cerr << "\tWebserv : fcntl: " << strerror(errno) << std::endl;
		close(Socket);
		return (-1);
	}

	new_event.events = EPOLLIN | EPOLLHUP | EPOLLRDHUP;
	new_event.data.fd = this->Socket;
	if (epoll_ctl(epoll_fd, EPOLL_CTL_ADD, this->Socket, &new_event) == -1) {
		perror("Epoll ctl failed");
		return (1);
	}
	return (0);
}

string Server::getHostName() const {
	return HostName;	
}

string Server::getPort() const {
	return Port;
}

vector<string>	Server::getIndexFiles() const {
	return IndexFiles;
}

vector<string> Server::getServerNames() const {
	return ServerNames;
}

long Server::getClientMaxBodySize() const {
	return ClientMaxBodySize;
}

vector<Route> Server::getRoutes() const {
	return _Routes;
}

string Server::getErrorPage(int status) const {
	if (status == 404)
		return ErrorPage404;
	else if (status == 403)
		return ErrorPage403;
	else if (status == 500)
		return ErrorPage500;
	return "";
}

string	Server::getRoot() const {
	return Root;
}

void	Server::setRoot(string root) {
	Root = root;
}

void	Server::setClientMaxBodySize(long size) {
	ClientMaxBodySize = size;
}

void	Server::setServerNames(vector<string> names) {
	for (size_t i = 0; i < names.size(); i++) {
		ServerNames.push_back(trim(names[i]));
	}
}

void	Server::setRoutes(vector<Route> routes) {
	_Routes = routes;
}

pair<string, int>	Server::getRedirectUrl() const {
	return _redirectionUrl;
}

bool	Server::isRouteExist(string route) {
	for (size_t i = 0; i < _Routes.size(); i++) {
		if (_Routes[i].getRouteName() == "\"" + route + "\"") 
			return true;
	}
	return false;
}

Route *Server::getRouteFromUri(string uri) {

	if (uri.empty())
		return NULL;
	if (uri[uri.length() - 1] == '/' && uri != "/") 
		uri = uri.substr(0, uri.length() - 1);

    for (size_t i = 0; i < _Routes.size(); i++) {

        if (_Routes[i].getRouteName() == string("\"" + uri + "\"")) {

            return &_Routes[i];
        }
    }

	string fileFromUri = uri.substr(uri.find_last_of('/') + 1);
	string uriWithoutFile = cpp11_replace(uri, fileFromUri, "");

	if (uriWithoutFile == "/")
	{

		return &_Routes[0];
	}
	return getRouteFromUri(uriWithoutFile);

}

void Server::setProperty(const string &key, string value) {
	if (key == "host") HostName = value;
	else if (key == "port") Port = value;
	else if (key == "server_names") setServerNames(split(value, ','));
	else if (key == "root") Root = value;
	else if (key == "index")
	{
		vector<string> indexes = split(value, ',');
		for (size_t i = 0; i < indexes.size(); i++)
			IndexFiles.push_back(trim(indexes[i]));
	}
	else if (key == "client_max_body_size") {
		string size = value.substr(0, value.length()-1);
		char unit = value[value.length()-1];
		ClientMaxBodySize = atol(size.c_str());
		if (unit == 'M') ClientMaxBodySize *= 1024 * 1024;
		else if (unit == 'K') ClientMaxBodySize *= 1024;
	}
	else if (key == "error_page_404") ErrorPage404 = "/tmp/www/" + value;
	else if (key == "error_page_403") ErrorPage403 = "/tmp/www/" + value;
	else if (key == "error_page_500") ErrorPage500 = "/tmp/www/" + value;	
	else if (key == "return") {

		vector<string> parts = split(value, ',');
		if (parts.size() != 2)
			throw runtime_error("\033[31m Invalid return value: " + value + "\nExpected format: \"return <url>, <status_code>\"");
		string url = parts[0];
		int status_code = atoi(parts[1].c_str());
		if (status_code < 100 || status_code > 599)
			throw runtime_error("\033[31m Invalid status code: " + parts[1]);
		if (url.find("http://") != string::npos || url.find("https://") != string::npos)
			_redirectionUrl = make_pair(url, status_code);
		else
			throw runtime_error("\033[31m Invalid URL: " + url + "\nRedirection URL should be a valid URL, e.g: http://example.com");
	}
	else {
		throw runtime_error("\033[31m Unknown property: " + key);
	}

}

void Server::updateAddress() {
    memset(&Address, 0, sizeof(Address));
    Address.sin_family = AF_INET;
    Address.sin_addr.s_addr = INADDR_ANY;
	Address.sin_port = htons(69);
}

void Server::addRoute(const Route& route) {
	_Routes.push_back(route);
}

ostream &operator<<(ostream &out, const Server &server) {
	out << "Host: " << server.getHostName() << endl;
	out << "Root: " << server.getRoot() << endl;
	out << "Port: " << server.getPort() << endl;
	out << "Server names: ";
	vector<string> names = server.getServerNames();
	for (size_t i = 0; i < names.size(); i++) {
		out << names[i];
		if (i < names.size() - 1) out << ", ";
	}
	out << endl;
	out << "Client max body size: " << server.getClientMaxBodySize() << endl;
	out << "_Routes:" << endl;
	vector<Route> routes = server.getRoutes();
	for (size_t i = 0; i < routes.size(); i++) {
		Route route = routes[i];
		out << "  Route " << i + 1 << ":" << endl;
		out << "    IndexFiles size: " << route.getRouteIndexFiles().size() << endl;
		out << "    Directory listing: " << (route.getRouteDirectoryListing() ? "on" : "off") << endl;
		out << "    Allowed methods: ";
		if (route.getRouteGETMethod()) out << "GET ";
		if (route.getRoutePOSTMethod()) out << "POST";
		out << endl;
		out << "    Upload store: " << route.getUploadStore() << endl;
		out << "    Client max body size: " << route.getClientMaxBodySize() << endl;
	}
	out << "Redirection URL: " << server.getRedirectUrl().first << ", status code: " << server.getRedirectUrl().second << endl;
	out << "Server names: ";
	vector<string> serverNames = server.getServerNames();
	for (size_t i = 0; i < serverNames.size(); i++) {
		out << serverNames[i];
		if (i < serverNames.size() - 1) out << ", ";
	}
	out << endl;
	out << "Index files: ";
	vector<string> indexFiles = server.getIndexFiles();
	for (size_t i = 0; i < indexFiles.size(); i++) {
		out << indexFiles[i];
		if (i < indexFiles.size() - 1) out << ", ";
	}
	out << endl;
	out << "Error page 404:" << server.getErrorPage(404) << endl;
	out << "Error page 403:" << server.getErrorPage(403) << endl;
	out << "Error page 500:" << server.getErrorPage(500) << endl;
	return out;
}

void	Server::addFriend(Server& server) {
	_ServerFriends.push_back(server);
}

vector<Server> Server::getFriends() const {
	return _ServerFriends;
}

bool Server::serverHasRootRoute() const {
	for (size_t i = 0; i < _Routes.size(); i++) {
		string RouteName = _Routes[i].getRouteName();
		if (RouteName == "\"/\"") {
			return true;
		}
	}
	return false;
}

void Server::CheckFiles()
{

	cout << bold << green << " -------- Files checked for friend " << def << endl;
    string rootPath = "/tmp/www/" + this->getRoot();
    if (serverHasRootRoute() == false) {
		throw runtime_error("\033[31m Server must have a default route \"/\"");
	}
	vector<Route> routes = this->getRoutes();
	for (size_t k = 0; k < routes.size(); k++)
	{
		routes[k].CheckFiles(this->getRoot());
	}
    struct stat s;
    if (stat(rootPath.c_str(), &s) == 0) {
        if (!(s.st_mode & S_IFDIR)) {
            throw runtime_error("\033[31m Path is not a directory: " + rootPath);
        }

        if (access(rootPath.c_str(), R_OK) != 0) {
            throw runtime_error("\033[31m No read access to directory");
        }

    } else {
        throw runtime_error("\033[31m Folder does not exist: " + rootPath);
    }
}