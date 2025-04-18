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
void	WebServer::run(){
	/*hardcoded tests*/



	/*  hna cancrew server wahd (socket ....)   */
	int server_fd = socket(AF_INET, SOCK_STREAM, 0);
	if (server_fd == -1)
	{
		cerr << "Error creating socket" << endl;
		exit(1);
	}
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
	cout << bold << green << "Server created succefully" << def << endl;


	while (1)
	{
		int client_fd = accept(server_fd, NULL, NULL);
		if (client_fd == -1)
		{
			cerr << "Error accepting connection" << endl;
			continue;
		}
		cout << bold << green << "Client connected" << def << endl;
		// this->RunningSockets.push_back(client_fd);

		//recive the request from the client
		char buffer[1024];
		int bytes_received = recv(client_fd, buffer, sizeof(buffer) - 1, 0);
		if (bytes_received == -1)
		{
			cerr << "Error receiving data" << endl;
			close(client_fd);
			continue;
		}

		buffer[bytes_received] = '\0';
		cout << bold << green << "Request received:\n\n" << buffer << def << endl << endl;
		//parse the request
		RequestProccessor req(buffer, "8081", server);
		cout << bold << green << "Request parsed: " << req << def << endl;
		
		POSTResponse postResponse(&req);
		cout << postResponse.generateResponse() << endl;
	}
	
	

	
	// RequestProccessor req("POST /index.html HTTP/1.1\nHost: www.axample.com\n", "8081");
	// cout << bold << green << req << def << endl;
	// File file("error/403.html");


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
int		WebServer::handleNewConnection(Server& server){
	(void)server;
	
	return 0;
}

/*
	Handle the client data, meaning when the I/O events are ready to read from the client
*/
int		WebServer::handleClientData(int client_fd, Server& server){
	(void)client_fd;
	(void)server;
	
	return 0;
}

/*
	Find the server that the client is trying to connect to
	`Simply a getter for one server in the Servers vector`
*/
Server* WebServer::findServerByHost(const RequestProccessor& req){
	(void)req;
	
	return nullptr;
}
