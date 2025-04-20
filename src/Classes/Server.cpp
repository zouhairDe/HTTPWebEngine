
#include "Server.hpp"
#include <cstring>
#include <string>
#include <cstddef>

Server::Server(string hostname, string port, string root)
	: HostName(hostname), Port(port), Root(root), ClientMaxBodySize(-1) {
}

Server::~Server() { }
	
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

string Server::getErrorPage() const {
	return ErrorPage;
}

string	Server::getRoot() const {
	return Root;
}

void	Server::setRoot(string root) {
	Root = root;
}

void	Server::setErrorPage(string pages) {
	ErrorPage = pages;
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

Route *Server::getRouteFromUri(string uri) {
    for (size_t i = 0; i < _Routes.size(); i++) {
        if (_Routes[i].getRouteName() == string("\"" + uri + "\"")) {
            cerr << "Route found from *Server::getRouteFromUri(): " << _Routes[i].getRouteName() << endl;
            return &_Routes[i];
        }
    }
    size_t lastSlash = uri.find_last_of('/');
    if (lastSlash != string::npos && uri != "/") {
        uri = uri.substr(0, lastSlash);
        return getRouteFromUri(uri);
    }
    return NULL;
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
		char unit = value[value.length()-1];//if unit is not a number consider it bytes
		ClientMaxBodySize = atol(size.c_str());
		if (unit == 'M') ClientMaxBodySize *= 1024 * 1024;
		else if (unit == 'K') ClientMaxBodySize *= 1024;
	}
	else if (key == "error_page_404") ErrorPage = "/tmp/www/" + value;
	
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
	out << "Error pages:" << server.getErrorPage() << endl;
	return out;
}

void	Server::addFriend(Server& server) {
	_ServerFriends.push_back(server);
}

vector<Server> Server::getFriends() const {
	return _ServerFriends;
}

void Server::CheckFiles()
{
	//check if the folder exists and we have access to it
	//then check the existence of the files bhal err_pages
	/*
		Define a directory or a file from where the file should be searched (for example,
		if url /kapouet is rooted to /tmp/www, url /kapouet/pouic/toto/pouet is
		/tmp/www/pouic/toto/pouet).
	*///so i dont think this is the thing
	//TODO: check hta error pages path w routesRoot path ikono bhal def w fix python script dzb
    string rootPath = "/tmp/www/" + this->getRoot();
    
    struct stat s;
    if (stat(rootPath.c_str(), &s) == 0) {
        if (!(s.st_mode & S_IFDIR)) {
            throw runtime_error("\033[31m Path is not a directory: " + rootPath);
        }
        
        if (access(rootPath.c_str(), R_OK) != 0) {
            throw runtime_error("\033[31m No read access to directory");
        }
		
		if (this->getErrorPage().empty())
			throw runtime_error("\033[31m Server must have an 404 error page");//TA NRUNNI NGNIX W NCHOFO HADI WACH S7I7A
			
		if (access(string(this->getErrorPage()).c_str(), R_OK) != 0)
			throw runtime_error("\033[31m No read access to 404 error page");
        
    } else {
        throw runtime_error("\033[31m Folder does not exist: " + rootPath);
    }
}
