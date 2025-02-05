
#include "Server.hpp"

Server::Server() : ClientMaxBodySize(-1) { }

Server::~Server() { }
	
string Server::getHostName() const {
	return HostName;	
}

string Server::getPort() const {
	return Port;
}

vector<string> Server::getServerNames() const {
	return ServerNames;
}

long Server::getClientMaxBodySize() const {
	return ClientMaxBodySize;
}

vector<Route> Server::getRoutes() const {
	return Routes;
}

map<string, string> Server::getErrorPages() const {
	return ErrorPages;
}

string	Server::getRoot() const {
	return Root;
}

void	Server::setRoot(string root) {
	Root = root;
}

void	Server::setErrorPages(map<string, string> pages) {
	ErrorPages = pages;
}

void	Server::setClientMaxBodySize(long size) {
	ClientMaxBodySize = size;
}

void	Server::setServerNames(vector<string> names) {
	ServerNames = names;
}

void	Server::setRoutes(vector<Route> routes) {
	Routes = routes;
}

void Server::setProperty(const string& key, string value) {
	if (key == "host") HostName = value;
	else if (key == "port") Port = value;
	else if (key == "server_names") setServerNames(split(value, ','));
	else if (key == "root") Root = value;
	else if (key == "client_max_body_size") {
		string size = value.substr(0, value.length()-1);
		char unit = value[value.length()-1];//if unit is not a number consider it bytes
		ClientMaxBodySize = atol(size.c_str());
		if (unit == 'M') ClientMaxBodySize *= 1024 * 1024;
		else if (unit == 'K') ClientMaxBodySize *= 1024;
	}
	else if (key.find("error_page_") != string::npos) {
		string code = key.substr(10);
		ErrorPages[code] = value;
	}
}

void Server::addRoute(const Route& route) {
	Routes.push_back(route);
}

friend ostream &Server::operator<<(std::ostream &out, const Server &server) {
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
	out << "Routes:" << endl;
	vector<Route> routes = server.getRoutes();
	for (size_t i = 0; i < routes.size(); i++) {
		Route route = routes[i];
		out << "  Route " << i + 1 << ":" << endl;
		out << "    Root: " << route.getRouteRoot() << endl;
		out << "    Index: " << route.getRouteIndex() << endl;
		out << "    Directory listing: " << (route.getRouteDirectoryListing() ? "on" : "off") << endl;
		out << "    Allowed methods: ";
		if (route.getRouteGETMethod()) out << "GET ";
		if (route.getRoutePOSTMethod()) out << "POST";
		out << endl;
		out << "    Upload store: " << route.getUploadStore() << endl;
		out << "    Client max body size: " << route.getClientMaxBodySize() << endl;
	}
	out << "Error pages:" << endl;
	map<string, string> errorPages = server.getErrorPages();
	for (map<string, string>::const_iterator it = errorPages.begin(); it != errorPages.end(); it++) {
		out << "  " << it->first << ": " << it->second << endl;
	}
	return out;
}
