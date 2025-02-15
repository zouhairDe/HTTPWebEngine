
#include "Server.hpp"

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

string	Server::getIndexFile() const {
	return IndexFile;
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
	Routes = routes;
}


void Server::setProperty(const string &key, string value) {
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
	else if (key == "error_page_404") ErrorPage = "/tmp/www/" + value;
	
}

void Server::updateAddress() {
    memset(&Address, 0, sizeof(Address));
    Address.sin_family = AF_INET;
    Address.sin_addr.s_addr = INADDR_ANY;
    Address.sin_port = htons(stoi(Port));
}

void Server::addRoute(const Route& route) {
	Routes.push_back(route);
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
	out << "Routes:" << endl;
	vector<Route> routes = server.getRoutes();
	for (size_t i = 0; i < routes.size(); i++) {
		Route route = routes[i];
		out << "  Route " << i + 1 << ":" << endl;
		out << "    Index: " << route.getRouteIndex() << endl;
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

int Server::kq = -1;

int Server::initKqueue() {
    if (kq == -1) {
        kq = kqueue();
        if (kq == -1) {
            cerr << red << "Error: kqueue() failed" << def << endl;
            return -1;
        }
    }
    return kq;
}

int Server::registerWithKqueue() {
    // Register server socket with kqueue
    EV_SET(&change_event, Socket, EVFILT_READ, EV_ADD, 0, 0, NULL);
    if (kevent(kq, &change_event, 1, NULL, 0, NULL) == -1) {
        cerr << red << "Error: kevent register failed" << def << endl;
        return -1;
    }
    return 0;
}

int Server::initSocket() {
    // Create socket
    Socket = socket(AF_INET, SOCK_STREAM, 0);
    if (Socket == -1) {
        cerr << red << "Error: socket creation failed: " << strerror(errno) << def << endl;
        return 1;
    }
    
    // Set socket options
    int opt = 1;
    int keepalive = 1;
    int keepcnt = 3;
    int keepidle = 1;
    int keepintvl = 1;
    
    if (setsockopt(Socket, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) == -1 ||
        setsockopt(Socket, SOL_SOCKET, SO_KEEPALIVE, &keepalive, sizeof(keepalive)) == -1 ||
        setsockopt(Socket, IPPROTO_TCP, TCP_KEEPCNT, &keepcnt, sizeof(keepcnt)) == -1 ||
        setsockopt(Socket, IPPROTO_TCP, TCP_KEEPIDLE, &keepidle, sizeof(keepidle)) == -1 ||
        setsockopt(Socket, IPPROTO_TCP, TCP_KEEPINTVL, &keepintvl, sizeof(keepintvl)) == -1) {
        cerr << red << "Error: setsockopt failed: " << strerror(errno) << def << endl;
        close(Socket);
        return 1;
    }

    // Set non-blocking mode
    int flags = fcntl(Socket, F_GETFL, 0);
    if (flags == -1 || fcntl(Socket, F_SETFL, flags | O_NONBLOCK) == -1) {
        cerr << red << "Error: fcntl failed: " << strerror(errno) << def << endl;
        close(Socket);
        return 1;
    }

    cout << green << "Socket initialized with fd: " << Socket << def << endl;
    return 0;
}

int Server::bindSocket() {
    memset(&Address, 0, sizeof(Address));
    Address.sin_family = AF_INET;
    Address.sin_port = htons(atoi(Port.c_str()));
    Address.sin_addr.s_addr = htonl(INADDR_ANY);  // Accept on all interfaces

    if (bind(Socket, (struct sockaddr *)&Address, sizeof(Address)) == -1) {
		//check if it failed because the port is already in use we return 0
		// if (errno == EADDRINUSE)// if its already in use nhandliwh somehowelse
        cerr << red << "Error: bind failed: " << strerror(errno) << def << endl;
        close(Socket);
        return 1;
    }

    cout << green << "Socket bound to port " << Port << def << endl;
    return 0;
}

int Server::listenSocket() {
    if (listen(Socket, SOMAXCONN) == -1) {
        cerr << red << "Error: listen failed: " << strerror(errno) << def << endl;
        close(Socket);
        return 1;
    }
    cout << blue << "Server is listening on port: " << Port << def << endl;
    return 0;
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
			throw runtime_error("\033[31m Server must have an 404 error page");
			
		if (access(string(this->getErrorPage()).c_str(), R_OK) != 0)
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
			// if (r.getRouteRoot() != this->getRoot())
			// 	throw runtime_error("\033[31m Route / must have the same root as the server");
			if (r.getRouteIndex() != this->getIndexFile())
				throw runtime_error("\033[31m Route / must have the same index as the server");
		}
		r.CheckFiles();
	}
}
