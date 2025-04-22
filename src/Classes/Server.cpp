
#include "Server.hpp"

Server::Server(string hostname, string port, string root)
	: HostName(hostname), Port(port), Root(root), ClientMaxBodySize(-1), Socket(-1) {
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
		this->ErrorPage = server.ErrorPage;
		this->_Routes = server._Routes;
		this->_ServerFriends = server._ServerFriends;
	}
	return (*this);
}

int Server::init(int epoll_fd) {
	int flags, opt;
	struct sockaddr_in server_addr;
	struct epoll_event new_event;
	
	this->Socket = socket(AF_INET, SOCK_STREAM, 0);
	if (this->Socket == -1) {
		cerr << "Error creating socket" << endl;
		return (1);
	}
	flags = fcntl(this->Socket, F_GETFL, 0);
	if (fcntl(this->Socket, F_SETFL, flags | O_NONBLOCK) == -1) {
		cerr << "Error setting socket to non-blocking" << endl;
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
	if (listen(this->Socket, 5) == -1) {
		cerr << "Error listening on socket" << endl;
		return (1);
	}

	new_event.events = EPOLLIN | EPOLLET;
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
	cout << "	URI: " << uri << endl;
    for (size_t i = 0; i < _Routes.size(); i++) {
        if (_Routes[i].getRouteName() == string("\"" + uri + "\"")) {
            return &_Routes[i];
        }
    }
	string fileFromUri = uri.substr(uri.find_last_of('/') + 1);
	string uriWithoutFile = uri.substr(0, uri.find_last_of('/'));
	if (uriWithoutFile.empty())
		uriWithoutFile = "/";
	for (size_t i = 0; i < _Routes.size(); i++) {
		if (_Routes[i].getRouteName() == string("\"" + uriWithoutFile + "\"")) {
			return &_Routes[i];
		}
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
