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
	cout << bold << green << "SERVER ON" << def << endl;

	while (true) {
        int client_socket = accept(server_fd, NULL, NULL);
        if (client_socket == -1) {
            cerr << "Error accepting connection" << endl;
            continue;
        }
        cout << bold << green << "CONNECTION ESTABLISHED" << def << endl;

		bool connected = true;
		string request;
		
		while (connected) {
        	char buffer[1024];
			for (int i = 0; i < 1024; i++) buffer[i] = '\0';
			int bytes_received = recv(client_socket, buffer, sizeof(buffer) - 1, 0);
			if (bytes_received == 0) {
				connected = false;
				break;
			} else if (bytes_received == -1) {
				cerr << "Error receiving data" << endl;
				break;
			}
			buffer[bytes_received] = '\0';
			string buffer_string(buffer, bytes_received);
			request += buffer_string;
				/* HNA ANHSBO BODY TOTAL SIZE - HEADER SIZE == CONTENT-LENGTH */
			if (request.find("\r\n\r\n") != string::npos) {
				size_t contentLengthPos = request.find("Content-Length: ");
				if (contentLengthPos != string::npos) {
					size_t endOfLine = request.find("\r\n", contentLengthPos);
					std::string contentLengthStr = request.substr(
						contentLengthPos + 16, // "Content-Length: " XXX
						endOfLine - (contentLengthPos + 16)
					);
					int contentLength = std::atoi(contentLengthStr.c_str());
					size_t bodyStartPos = request.find("\r\n\r\n") + 4;
					int bodyBytesReceived = request.length() - bodyStartPos;
					if (bodyBytesReceived >= contentLength) {
						connected = false;
						break;
					}
				}
				else {
					// For requests without Content-Length (like GET), 
					// consider them complete once we have headers
					if (request.find("GET") == 0 || request.find("HEAD") == 0) {
						connected = false;
						break;
					}
				}
			}
		}



        RequestProccessor req(request, "8081", server);
		cout << bold << green << "Request parsed:\n" << request << def << endl;
		// req.debugRequest();
        
        if (req.getMethod() == "GET") {
			File *f = new File("./error/404.html");
            GETResponse getResponse(&req, f);
            string response = getResponse.generateResponse();
            send(client_socket, response.c_str(), response.length(), 0);
        } 
		else if (req.getMethod() == "POST") {
            POSTResponse postResponse(&req);
            string response = postResponse.generateResponse();
            send(client_socket, response.c_str(), response.length(), 0);
            
            if (!req.getFileContent().empty()) {
                string uploadPath = "./body/" + req.getStoreFileName();
                ofstream outFile(uploadPath.c_str(), ios::binary);
                if (outFile.is_open()) {
                    outFile.write(req.getFileContent().c_str(), req.getFileContent().length());
                    outFile.close();
                    cout << bold << green << "File saved: " << uploadPath << def << endl;
                } else {
                    cerr << "Failed to save uploaded file" << endl;
                }
            }
        }

        if (req.getConnection() == "close") {
			cout << bold << green << "CLOSED" << def << endl;
			close(client_socket);
			break;
		} else {
			cout << bold << green << "KEEP-ALIVE" << def << endl;
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
int		WebServer::handleNewConnection(Server& server){
	(void)server;
	
	return 0;
}

/*
	Handle the client data, meaning when the I/O events are ready to read from the client
*/
int		WebServer::handleClientData(int client_socket, Server& server){
	(void)client_socket;
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
