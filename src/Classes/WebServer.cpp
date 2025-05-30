/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   WebServer.cpp                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: zouddach <zouddach@student.1337.ma>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/02/06 15:32:33 by zouddach          #+#    #+#             */
/*   Updated: 2025/04/17 20:52:03 by zouddach         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

# include "WebServer.hpp"
# include "RequestProcessor.hpp"

#define MAX_EVENTS 32

WebServer::WebServer(char *filename)
{
	ConfigParser config(filename);
	ifstream file(filename);
	string line;
	int serverCount = 0; 
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
	file.close();
	config.setServerCount(serverCount);

	Servers = config.parseConfig(string(filename));
	if (Servers.empty())
	{
		cerr << "Error: No servers found in the configuration file." << endl;
		exit(1);
	}
	cout << def << endl
		 << endl; 
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
			server.setClientMaxBodySize(10 * 1024 * 1024); 
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

	for (size_t i = 0; i < Servers.size(); i++) {
		Server *server = &Servers[i];
		if (server->init(epoll_fd)) {
			cerr << "Error initializing server " << i << endl;
			close(epoll_fd);
			return ;
		}
	}

	map<int, RequestProcessor> requests;

	cout << bold << endl << "============== SERVER ON ==============" << def << endl << endl;

	while (true) {
		int event_count = epoll_wait(epoll_fd, events, MAX_EVENTS, 1000);
		if (event_count == -1) {
			perror("Epoll wait failed");
		}
		for (int i = 0; i < event_count; i++) {
			if (events[i].events & EPOLLERR) {
				deleteSocket(epoll_fd, events[i].data.fd);
				close(events[i].data.fd);
				if (requests.find(events[i].data.fd) != requests.end()) {
					requests.erase(events[i].data.fd);
				}
				continue ;
			} else if (events[i].events & EPOLLHUP) {
				deleteSocket(epoll_fd, events[i].data.fd);
				close(events[i].data.fd);
				if (requests.find(events[i].data.fd) != requests.end()) {
					requests.erase(events[i].data.fd);
				}
				continue ;
			} else if (events[i].events & EPOLLRDHUP) {
				deleteSocket(epoll_fd, events[i].data.fd);
				close(events[i].data.fd);
				if (requests.find(events[i].data.fd) != requests.end()) {
					requests.erase(events[i].data.fd);
				}
				continue ;
			}
			bool new_connection = false;
			for (size_t s = 0; s < Servers.size(); s++) {
				Server *server = &Servers[s];
				if (events[i].data.fd == server->Socket) {
					int client_socket = handleNewConnection(server->Socket, epoll_fd);
					if (client_socket == -1) {
						cerr << "Error accepting connection" << endl;
						continue ;
					}
					if (requests.size() >= MAX_CLIENTS) {
						cerr << "Max clients reached" << endl;
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
				if (requests[client_socket].responded() == false) {
					modifySocket(epoll_fd, client_socket, EPOLLIN | EPOLLOUT);
				} else {
					requests[client_socket].log();
					if (requests[client_socket].getConnection() == "close") {
						requests[client_socket].clear();
						deleteSocket(epoll_fd, client_socket);
						close(client_socket);
						requests.erase(client_socket);
					} else {
						requests[client_socket].clear();
					}
				}
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
	fcntl(client_socket, F_SETFL, O_NONBLOCK);

	int opt = 1;
	if (setsockopt(client_socket, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) == -1) {
		cerr << "Error setting socket options" << endl;
		close(client_socket);
		return -1;
	}

	new_event.events = EPOLLIN | EPOLLHUP | EPOLLRDHUP;
	new_event.data.fd = client_socket;
	if (epoll_ctl(epoll_fd, EPOLL_CTL_ADD, client_socket, &new_event) == -1) {
		perror("Epoll_ctl client failed");
		close(client_socket);
	}

	return (client_socket);
}