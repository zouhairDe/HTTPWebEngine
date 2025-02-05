
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

string Server::getErrorPages() const {
	return Not_found_ErrorPage;
}

string	Server::getRoot() const {
	return Root;
}

string	Server::getIndexFile() const {
	return IndexFile;
}

void	Server::setRoot(string root) {
	Root = root;
}

void	Server::setErrorPages(string page) {
	Not_found_ErrorPage = page;
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
	else if (key == "index") IndexFile = value;
	else if (key == "client_max_body_size") {
		string size = value.substr(0, value.length()-1);
		char unit = value[value.length()-1];//if unit is not a number consider it bytes
		ClientMaxBodySize = atol(size.c_str());
		if (unit == 'M') ClientMaxBodySize *= 1024 * 1024;
		else if (unit == 'K') ClientMaxBodySize *= 1024;
	}
	else if (key == "error_page_404") {
		Not_found_ErrorPage = value;
	}
}

void Server::addRoute(const Route& route) {
	Routes.push_back(route);
}

ostream &operator<<(std::ostream &out, const Server &server) {
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
	out << "Not Found error page:" << server.getErrorPages() << endl;
	return out;
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
		
		if (this->getErrorPages().empty())
			throw runtime_error("\033[31m Server must have an 404 error page");
			
		if (access(string("/tmp/www/" + this->getErrorPages()).c_str(), R_OK) != 0)
			throw runtime_error("\033[31m No read access to 404 error page");
        
    } else {
        throw runtime_error("\033[31m Folder does not exist: " + rootPath);
    }
	
	vector<Route> rs = this->getRoutes();
	for (size_t i = 0; i < rs.size(); i++)
	{
		Route &r = rs[i];
		if (r.getRouteName() == "\"/\"")
		{
			if (r.getRouteRoot() != this->getRoot())
				throw runtime_error("\033[31m Route / must have the same root as the server");
			if (r.getRouteIndex() != this->getIndexFile())
				throw runtime_error("\033[31m Route / must have the same index as the server");
		}
		r.CheckFiles();
	}
}