
#include "Server.hpp"

Server::Server(string hostname, string port, string root)
	: HostName(hostname), Port(port), Root(root), ClientMaxBodySize(-1) {

    memset(&Address, 0, sizeof(Address));
    Address.sin_family = AF_INET;
    Address.sin_addr.s_addr = INADDR_ANY;
    Address.sin_port = htons(stoi(Port));
	
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
	else if (key == "error_page_404") ErrorPage = "/tmp/www/" + value;
	
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
	out << "Error pages:" << server.getErrorPage() << endl;
	return out;
}

int	Server::initSocket() {
	/* 0 hia protocol, w ghaliban system kiyakhed 
	l appropriate one chnahoma lakhrin? lahoa3lam */
	Socket = socket(AF_INET, SOCK_STREAM, 0);
	if (Socket == -1) {
		cerr << "Error: socket creation failed" << endl;
		return (1);
	}
	cout << "Socket created successfully" << endl;
	cout << "File descriptor: " << Socket << endl;
	
	int opt = 1;
    if (setsockopt(Socket, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) == -1) {
        cerr << "Error: setsockopt failed" << endl;
        close(Socket);
        return (1);
    }
	cout << "Socket options set successfully" << endl;
	cout << "File descriptor: " << Socket << endl;
	return (0);
}

int	Server::bindSocket(void) {
	cout << "File descriptor: " << HostName << endl;
	if (bind(Socket, (struct sockaddr *)&Address, sizeof(Address)) == -1) {
        cerr << "Error: bind failed" << endl;
        close(Socket);
        return (1);
    }
    cout << "Socket bound successfully" << def << endl;
	return (0);
}

int	Server::listenSocket(void) {
	if (listen(Socket, 3) == -1) {
		cerr << "Error: listen failed" << endl;
		close(Socket);
		return 1;
	}
	cout << red << bold << blue << "Server is listening on port 8080..." << def << endl;
	return (0);
}

int	Server::communicate(void) {
	char buffer[1024];
	int client_fd;
	while (true) {

		struct sockaddr_in client_addr;
		socklen_t client_len = sizeof(client_addr);
		client_fd = accept(Socket, (struct sockaddr*)&client_addr, &client_len);
		if (client_fd == -1) {
			cerr << "Error: accept failed" << endl;
			close(Socket);
			return 1;
		}
		cout << "Client connected!" << def << endl;

        memset(buffer, 0, sizeof(buffer));
        ssize_t bytes_received = recv(client_fd, buffer, sizeof(buffer), 0);
        if (bytes_received <= 0) {
            cout << red << "Client disconnected (Empty body: chof dok 2 stora khawyin t7t akhir http header)" << def << endl;
            break;
        }
        cout << red << bold << blue << "Client: " << def << buffer << endl;
		
    }
	File file(this->getErrorPage());
	if (!file.exists()) {
		cerr << red << "Error: file not found: " << this->getErrorPage() << endl;
		close(client_fd);
		return (1);
	}
	
	string http_headers = 
		"HTTP/1.1 200 OK\n\
Date: Fri, 01 Jul 2022 12:00:00 GMT\n\
Server: Apache/2.4.41 (Ubuntu)\n\
Last-Modified: Mon, 13 Jun 2022 10:00:00 GMT\n\
Content-Length: " + to_string(file.getSize()) + "\n\
Content-Type: text/html; charset=UTF-8\n\
Connection: keep-alive\n";
	
	cerr << red << "HTTP headers: " << http_headers << endl;

	send(client_fd, http_headers.c_str(), http_headers.length(), 0);
	send(client_fd, file.getData(), file.getSize(), 0);
	close(client_fd);

	return (0);
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
			if (r.getRouteRoot() != this->getRoot())
				throw runtime_error("\033[31m Route / must have the same root as the server");
			if (r.getRouteIndex() != this->getIndexFile())
				throw runtime_error("\033[31m Route / must have the same index as the server");
		}
		r.CheckFiles();
	}
}
