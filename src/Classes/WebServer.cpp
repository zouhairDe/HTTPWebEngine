
#include "WebServer.hpp"

WebServer::WebServer(char *filename)
{
	// khas ncheckiw b3da wach kayn error line fl config file
	ConfigParser config(filename);
	ifstream file(filename);
	string line;
	int serverCount = 0; // This is to count servers for progress bar
	while (getline(file, line))
	{
		line = trim(line);
		if (line.empty() || line[0] == '#')
			continue;
		if (config.isServerBlock(line))
		{
			serverCount++;
		}
	}
	cerr << "Server count: " << serverCount << endl;
	file.close();
	config.setServerCount(serverCount);

	Servers = config.parseConfig(string(filename), false);
	// setting the default server "config/default.conf"
	DefaultServer = config.parseConfig("config/default.conf", true)[0];
	cout << def << endl
		 << endl; // Move to the next line after the progress bar is complete
	changeEmptyValues();
}

WebServer::~WebServer() {}

void WebServer::changeEmptyValues()
{ // must have properties homa host, port 404err_page, index and root Route
	// cerr << "Changing Empty values for servers" << endl;
	for (size_t i = 0; i < this->Servers.size(); i++)
	{
		Server &server = this->Servers[i];
		if (server.getRoot().empty())
			throw runtime_error("\033[31m Server: " + cpp11_toString(i + 1) + " must have a Root");
		if (server.getHostName().empty())
			throw runtime_error("\033[31m Server: " + cpp11_toString(i + 1) + " must have a HostName");
		if (server.getPort().empty())
			throw runtime_error("\033[31m Server: " + cpp11_toString(i + 1) + " must have a Port to listen to");
		if (server.getIndexFile().empty())
			throw runtime_error("\033[31m Server: " + cpp11_toString(i + 1) + " must have an Index File");
		if (server.getClientMaxBodySize() == -1)
			server.setClientMaxBodySize(this->DefaultServer.getClientMaxBodySize());
		if (server.getRoutes().empty())
			server.setRoutes(this->DefaultServer.getRoutes());
		if (server.getErrorPage().empty())
			throw runtime_error("\033[31m Server: " + cpp11_toString(i + 1) + " must have an 404 error page");
	}
}

void WebServer::printData()
{
	cout << "Default server:" << endl;
	cout << this->DefaultServer;
	for (size_t i = 0; i < this->Servers.size(); i++)
	{
		cout << this->Servers[i] << endl;
		cout << "---------------------------------" << endl;
	}
}

void WebServer::CheckFiles()
{
	for (size_t i = 0; i < this->Servers.size(); i++)
	{
		Server &server = this->Servers[i];
		cout << bold << green << " -------- Checking files for server " << i + 1 << def << endl;
		server.CheckFiles();
	}
}

void WebServer::run()
{
	// init kqueue
	if (Server::initKqueue() == -1)
	{
		throw runtime_error("Failed to initialize kqueue");
	}

	// init all servers
	for (size_t i = 0; i < Servers.size(); i++)
	{
		Server &server = Servers[i];
		cout << bold << green << " -------- Starting server " << i + 1 << def << endl;
		/* Check if this IP:Port are already in our RunningServers: vector<string> if yes continue*/
		if (find(this->RunningServers.begin(), this->RunningServers.end(), server.getHostName() + ":" + server.getPort()) != this->RunningServers.end())
		{
			cout << blue << "Server " << server.getHostName() << ":" << server.getPort() << " is already running" << def << endl;
			continue;
		}
		if (server.initSocket() != 0 ||
			server.bindSocket() != 0 ||
			server.listenSocket() != 0 ||
			server.registerWithKqueue() != 0)
		{
			throw runtime_error("Failed to initialize server " + to_string(i + 1));
		}
		this->RunningServers.push_back(server.getHostName() + ":" + server.getPort());
	}
	// maybe check servers size before running, ila 0 stop throw runtime_error("No servers to run");
	this->communicate();
}

int WebServer::communicate()
{
	// char buffer[1024];
	struct timespec timeout = {5, 0}; // 5 second timeout

	while (true)
	{
		// Wait for events
		int new_events = kevent(Server::kq, NULL, 0, Servers[0].event_list, 32, &timeout);
		if (new_events == -1)
		{
			if (errno == EINTR)
				continue;
			cerr << red << "Error: kevent wait failed: " << strerror(errno) << def << endl;
			return -1;
		}

		// Process events
		for (int i = 0; i < new_events; i++)
		{
			int event_fd = Servers[0].event_list[i].ident;

			// Find which server owns this socket
			Server *server = getServerBySocket(event_fd);
			if (!server)
			{
				// Check if it's a client socket
				for (vector<Server>::iterator it = Servers.begin(); it != Servers.end(); ++it)
				{
					if (event_fd != it->Socket)
					{
						server = &(*it);
						break;
					}
				}
				if (!server)
					continue;
			}

			// If event is on server socket, accept new connection
			if (event_fd == server->Socket)
			{
				handleNewConnection(*server);
			}
			else
			{
				handleClientData(event_fd, *server);
			}
		}
	}
	return 0;
}

Server *WebServer::getServerBySocket(int socket_fd)
{
	for (size_t i = 0; i < Servers.size(); i++)
	{
		if (socket_fd == Servers[i].Socket)
		{
			return &Servers[i];
		}
	}
	// for (Server& server : Servers) {
	//     if (server.Socket == socket_fd) {
	//         return &server;
	//     }
	// }
	return nullptr;
}

int WebServer::handleNewConnection(Server &server)
{
	struct sockaddr_in client_addr;
	socklen_t client_len = sizeof(client_addr);
	int client_fd = accept(server.Socket, (struct sockaddr *)&client_addr, &client_len);

	if (client_fd == -1)
	{
		cerr << red << "Error: accept failed" << def << endl;
		return -1;
	}

	// Register new client for reading
	struct kevent client_event;
	EV_SET(&client_event, client_fd, EVFILT_READ, EV_ADD, 0, 0, NULL);
	if (kevent(Server::kq, &client_event, 1, NULL, 0, NULL) == -1)
	{
		cerr << red << "Error: failed to register client" << def << endl;
		close(client_fd);
		return -1;
	}

	cout << green << "Client connected!" << def << endl;
	return 0;
}

int WebServer::handleClientData(int client_fd, Server &server)
{
	char buffer[1024];
	memset(buffer, 0, sizeof(buffer));

	ssize_t bytes_received = recv(client_fd, buffer, sizeof(buffer), 0);
	if (bytes_received <= 0)
	{
		if (errno == EAGAIN || errno == EWOULDBLOCK)
		{
			return 0;
		}
		cerr << red << "Error receiving data: " << strerror(errno) << def << endl;
		close(client_fd);
		return -1;
	}

	// Go to logs/ and create a folder with this server's hostname and inside it create a file with the current date and time+_Request then add the received data
	string logsPath = "logs/" + server.getHostName();
	if (mkdir(logsPath.c_str(), 0777) == -1 && errno != EEXIST)
	{
		cerr << red << "Error: failed to create logs directory" << def << endl;
		close(client_fd);
		return -1;
	}

	// creating the file
	time_t now = time(0);
	tm *ltm = localtime(&now);
	string date = to_string(1900 + ltm->tm_year) + "-" +
				  to_string(1 + ltm->tm_mon) + "-" +
				  to_string(ltm->tm_mday) + "_" +
				  to_string(ltm->tm_hour) + ":" +
				  to_string(ltm->tm_min) + ":" +
				  to_string(ltm->tm_sec);
	string filePath = logsPath + "/" + date + "_Request";

	// Open file once with append mode
	ofstream reqFile(filePath, ios::out | ios::app);
	if (!reqFile.is_open())
	{
		cerr << red << "Error: Cannot open log file" << def << endl;
		close(client_fd);
		return -1;
	}

	// Write request
	reqFile << "----Request----\n"
			<< buffer << "\n\n";

	// Handle response
	File file(server.getErrorPage());
	if (!file.exists())
	{
		cerr << red << "Error: file not found: " << server.getErrorPage() << def << endl;
		reqFile.close();
		close(client_fd);
		return -1;
	}

	string http_headers = generateHttpHeaders(file, 404);

	// Send response
	if (send(client_fd, http_headers.c_str(), http_headers.length(), 0) == -1 ||
		send(client_fd, file.getData(), file.getSize(), 0) == -1)
	{
		cerr << red << "Error: Failed to send response" << def << endl;
		reqFile.close();
		close(client_fd);
		return -1;
	}

	// Log response
	reqFile << "----Response----\n"
			<< http_headers;
	if (file.getData() && file.getSize() > 0)
	{
		reqFile.write(file.getData(), file.getSize());
	}
	reqFile.close();
	return 0;
}