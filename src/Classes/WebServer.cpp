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
# include "RequestProcessor.hpp"

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

int modifySocket(int epoll_fd, int socket, int events)
{
    struct epoll_event ev;
    ev.events = events;
    ev.data.fd = socket;
    if (epoll_ctl(epoll_fd, EPOLL_CTL_MOD, socket, &ev) == -1) {
        perror("Epoll ctl modify failed");
        return (1);
    }
    return (0);
}

int deleteSocket(int epoll_fd, int socket)
{
	if (epoll_ctl(epoll_fd, EPOLL_CTL_DEL, socket, NULL) == -1) {
		perror("Epoll ctl delete failed");
		return (1);
	}
	return (0);
}

void	WebServer::run(){
	struct epoll_event events[MAX_EVENTS];

	int epoll_fd = epoll_create1(0);
	if (epoll_fd == -1) {
		perror("Epoll create failed");
		return ;
	}
	/*  hna cancrew server wahd (socket ....)   */
	
	for (size_t i = 0; i < Servers.size(); i++) {
		Server *server = &Servers[i];
		if (server->init(epoll_fd)) {
			cerr << "Error initializing server " << i << endl;
			return ;
		} else {
			cout << bold << green << "Server " << i << " initialized" << def << endl;
		}
	}

	map<int, RequestProcessor> requests;

	cout << bold << endl << "============== SERVER ON ==============" << def << endl << endl;

	while (true) {
		int event_count = epoll_wait(epoll_fd, events, MAX_EVENTS, -1);
		if (event_count == -1) {
			perror("Epoll wait failed");
		}
		for (int i = 0; i < event_count; i++) {
			bool new_connection = false;
			for (size_t s = 0; s < Servers.size(); s++) {
				/* hna ancheckiw if dak server 3ndo return (redirectionUrl 9adlo response dialo nichan w sindiga w nafs l haja hta l routes) */
				Server *server = &Servers[s];//new Server(Servers[s]);
				if (events[i].data.fd == server->Socket) {
					int client_socket = handleNewConnection(server->Socket, epoll_fd);
					if (requests.size() >= MAX_CLIENTS) {
						cerr << "Max clients reached" << endl;//return service unavailable later
						close(client_socket);
						continue ;
					}
					requests[client_socket] = RequestProcessor();
					requests[client_socket].setPort(server->getPort());

					requests[client_socket]._server = server;
					new_connection = true;
					break ;
				}
			}
			if (new_connection)
				continue ;
			int client_socket = events[i].data.fd;
			int status = requests[client_socket].receiveRequest(client_socket);
			if (requests[client_socket].received() || status > 0) {
				requests[client_socket].sendResponse();
				cout << "REQUEST ARRIVED" << endl;
				cout << bold << green << "SENT? " << requests[client_socket].responded() << def << endl;
				if (requests[client_socket].responded() == false) {
					modifySocket(epoll_fd, client_socket, EPOLLIN | EPOLLOUT | EPOLLET);
				} else {
					requests[client_socket].log();
					if (requests[client_socket].getConnection() == "close") {
						cout << bold << green << "CLOSED" << def << endl;
						requests[client_socket].clear();
						deleteSocket(epoll_fd, client_socket);
						close(client_socket);
						requests.erase(client_socket);
					} else {
						cout << bold << green << "KEEP-ALIVE" << def << endl;
						modifySocket(epoll_fd, client_socket, EPOLLIN | EPOLLET);
						requests[client_socket].clear();
					}
				}
			} else {
				modifySocket(epoll_fd, client_socket, EPOLLIN);
			}
		}
    }
}


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