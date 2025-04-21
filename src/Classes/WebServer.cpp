/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   WebServer.cpp                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: zouddach <zouddach@student.1337.ma>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/17 15:32:33 by zouddach          #+#    #+#             */
/*   Updated: 2025/04/17 20:52:03 by zouddach         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

# include "WebServer.hpp"
# include "RequestProccessor.hpp"
# include "GETResponse.hpp"
# include "POSTResponse.hpp"

#define MAX_EVENTS 32

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

	Servers = config.parseConfig(string(filename));
	cout << def << endl
		 << endl; // Move to the next line after the progress bar is complete
	changeEmptyValues();
}

WebServer::~WebServer() {}

void WebServer::changeEmptyValues()
{
	for (size_t i = 0; i < this->Servers.size(); i++)
	{
		Server &server = this->Servers[i];
		if (server.getRoot().empty())
			throw runtime_error("\033[31m Server: " + cpp11_toString(i + 1) + " must have a Root");
		if (server.getHostName().empty())
			throw runtime_error("\033[31m Server: " + cpp11_toString(i + 1) + " must have a HostName");
		if (server.getPort().empty())
			throw runtime_error("\033[31m Server: " + cpp11_toString(i + 1) + " must have a Port to listen to");
		if (server.getClientMaxBodySize() == -1)
			server.setClientMaxBodySize(10 * 1024 * 1024); // 10MB default li ndiro
		if (server.getErrorPage().empty())
			throw runtime_error("\033[31m Server: " + cpp11_toString(i + 1) + " must have an 404 error page");
	}
}

void WebServer::printData()
{
	cout << "----------- Servers: ------------:" << endl;
	for (size_t i = 0; i < this->Servers.size(); i++)
	{
		cout << this->Servers[i] << endl;
		cout << "---------------------------------" << def << endl;
		if (this->Servers[i].getFriends().size() > 0)
		{
			cout << bold << blue << "Friends of server " << i + 1 << def << endl;
			vector<Server> friends = this->Servers[i].getFriends();
			for (size_t j = 0; j < friends.size(); j++)
			{
				cout << friends[j] << endl;
				cout << "---------------------------------" << def << endl;
			}
		}
	}
}

void WebServer::CheckFiles()
{
	for (size_t i = 0; i < this->Servers.size(); i++)
	{
		Server &server = this->Servers[i];
		cout << bold << green << " -------- Checking files for server " << i + 1 << def << endl;
		server.CheckFiles();
		if (server.getFriends().size() > 0)
		{
			cout << bold << blue << " -------- Checking files for friends of server " << i + 1 << def << endl;
			vector<Server> friends = server.getFriends();
			for (size_t j = 0; j < friends.size(); j++)
			{
				cout << bold << green << " -------- Checking files for friend " << j + 1 << def << endl;
				friends[j].CheckFiles();
			}
		}
	}
}

/*
	The function responsable for ranning all the I/O functions
	doe the servers `Creating a socket for all of them
	and adding them to the WebServer::RunningSockets vector`
*/

// void	handleNewConnection(int server_fd, int epoll_fd)
// {
// }

// void handleClientData
// }

void	WebServer::run(){
	/*  hna cancrew server wahd (socket ....)   */
	int server_fd = socket(AF_INET, SOCK_STREAM, 0);
	if (server_fd == -1)
	{
		cerr << "Error creating socket" << endl;
		exit(1);
	}
	int flags = fcntl(server_fd, F_GETFL, 0);
	fcntl(server_fd, F_SETFL, flags | O_NONBLOCK);
	int opt = 1;
	if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) == -1)
	{
		cerr << "Error setting socket options" << endl;
		exit(1);
	}

	struct sockaddr_in server_addr;
	server_addr.sin_family = AF_INET;
	server_addr.sin_addr.s_addr = INADDR_ANY;
	server_addr.sin_port = htons(8081);
	if (bind(server_fd, (struct sockaddr *)&server_addr, sizeof(server_addr)) == -1)
	{
		cerr << "Error binding socket" << endl;
		exit(1);
	}
	if (listen(server_fd, 5) == -1)
	{
		cerr << "Error listening on socket" << endl;
		exit(1);
	}
	Server *server = &Servers[0];

	struct epoll_event new_event, events[MAX_EVENTS];
	int epoll_fd = epoll_create1(0);
	if (epoll_fd == -1) {
		perror("Epoll create failed");
		exit(EXIT_FAILURE);
	}
	new_event.events = EPOLLIN | EPOLLET;
	new_event.data.fd = server_fd;
	if (epoll_ctl(epoll_fd, EPOLL_CTL_ADD, server_fd, &new_event) == -1) {
		perror("Epoll ctl failed");
		exit(EXIT_FAILURE);
	}
	map<int, RequestProccessor> requests;

	cout << bold << endl << "============== SERVER ON ==============" << def << endl << endl;

	while (true) {
		int event_count = epoll_wait(epoll_fd, events, MAX_EVENTS, -1);
		// std::cout << bold << green << "Waiting for events..." << def << endl;
		// std::cout << bold << green << "Event count: " << event_count << def << endl;
		if (event_count == -1) {
			perror("Epoll wait failed");
			exit(EXIT_FAILURE);
		}
		for (int i = 0; i < event_count; i++) {
			if (events[i].data.fd == server_fd) {
				int client_socket = handleNewConnection(server_fd, epoll_fd);
				requests[client_socket] = RequestProccessor();
				continue ;
			}
			int client_socket = events[i].data.fd;
			bool done = requests[client_socket].receiveRequest(client_socket, "8081", server);
			if (done) {
				requests[client_socket].log();
				handleClientData(requests[client_socket]);
				if (requests[client_socket].getConnection() == "close") {
					if (epoll_ctl(epoll_fd, EPOLL_CTL_DEL, client_socket, NULL) == -1) {
						perror("Epoll ctl failed");
						exit(EXIT_FAILURE);
					};
					// cout << bold << green << "CLOSED" << def << endl;
					close(requests[client_socket].getSocket());
					requests[client_socket].clear();
					requests.erase(client_socket);
				} else {
					// cout << bold << green << "KEEP-ALIVE" << def << endl;
					requests[client_socket].clear();
				}
			}
		}
    }
}

/*
	Communicate with the clients which means this function will be responsable for
	reading and writing to the clients sockets after processing the request
*/
int		WebServer::communicate(){
	
	return 0;
}

/*
	Find the server that the client is trying to connect to
*/
Server* WebServer::getServerBySocket(int socket_fd){
	(void)socket_fd;
	return nullptr;
}

/*
	Handle the new connection from the client, meanning when a 
	new client connects to the server
*/
int		WebServer::handleNewConnection(int server_fd, int epoll_fd){
	
	struct epoll_event new_event;
	int client_socket = accept(server_fd, NULL, NULL);
	if (client_socket == -1) {
		cerr << "Error accepting connection" << endl;
		return -1;
	}
	int flags = fcntl(client_socket, F_GETFL, 0);
	fcntl(client_socket, F_SETFL, flags | O_NONBLOCK);
	new_event.events = EPOLLIN | EPOLLET;
	new_event.data.fd = client_socket;
	if (epoll_ctl(epoll_fd, EPOLL_CTL_ADD, client_socket, &new_event) == -1) {
		perror("Epoll_ctl client failed");
		close(client_socket);
	}

	// cout << bold << green << "NEW CONNECTION" << def << endl;

	return (client_socket);
}

/*
	Handle the client data, meaning when the I/O events are ready to read from the client
*/
int		WebServer::handleClientData(RequestProccessor &request) {
	if (request.getMethod() == "GET") {
		File *f = new File("./error/404.html");
		GETResponse getResponse(&request, f);
		string response = getResponse.generateResponse();
		send(request.getSocket(), response.c_str(), response.length(), 0);
	} else if (request.getMethod() == "POST") {
		POSTResponse postResponse(&request);
		string response = postResponse.generateResponse();
		send(request.getSocket(), response.c_str(), response.length(), 0);
		
		if (!request.getFileContent().empty()) {
			string uploadPath = "./body/" + request.getStoreFileName();
			ofstream outFile(uploadPath.c_str(), ios::binary);
			if (outFile.is_open()) {
				outFile.write(request.getFileContent().c_str(), request.getFileContent().length());
				outFile.close();
				// cout << bold << green << "File saved: " << uploadPath << def << endl;
			} else {
				cerr << "Failed to save uploaded file" << endl;
			}
		} else {
			cout << bold << red << "No file uploaded" << def << endl;
		}
	}

	return 1;
}

/*
	Find the server that the client is trying to connect to
	`Simply a getter for one server in the Servers vector`
*/
Server* WebServer::findServerByHost(const RequestProccessor& req){
	(void)req;
	
	return nullptr;
}
