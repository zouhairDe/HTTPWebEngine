
#include "WebServer.hpp"

WebServer::WebServer(char* filename) {
	// khas ncheckiw b3da wach kayn error line fl config file
	ConfigParser config(filename);
	ifstream file(filename);
	string line;
	int serverCount = 0;//This is to count servers for progress bar
	while (getline(file, line)) {
		line = trim(line);
		if (line.empty() || line[0] == '#') continue;
		if (config.isServerBlock(line)) {
			serverCount++;
		}
	}
	cerr << "Server count: " << serverCount << endl;
	file.close();
	config.setServerCount(serverCount);
	
	Servers = config.parseConfig(string(filename), false);
	//setting the default server "config/default.conf"
	DefaultServer = config.parseConfig("config/default.conf", true)[0];
	cout << def << endl << endl; // Move to the next line after the progress bar is complete
	changeEmptyValues();
}

WebServer::~WebServer() { }

void	WebServer::changeEmptyValues() {//must have properties homa host, port 404err_page, index and root Route
	// cerr << "Changing Empty values for servers" << endl;
	for (size_t i = 0; i < this->Servers.size(); i++)
	{
		Server &server = this->Servers[i];
		if (server.getRoot().empty())
			throw runtime_error("\033[31m Server: " + cpp11_toString(i + 1)  + " must have a Root");
		if (server.getHostName().empty())
			throw runtime_error("\033[31m Server: " + cpp11_toString(i + 1)  + " must have a HostName");
		if (server.getPort().empty())
			throw runtime_error("\033[31m Server: " + cpp11_toString(i + 1)  + " must have a Port to listen to");
		if (server.getIndexFile().empty())
			throw runtime_error("\033[31m Server: " + cpp11_toString(i + 1)  + " must have an Index File");
		if (server.getClientMaxBodySize() == -1)
			server.setClientMaxBodySize(this->DefaultServer.getClientMaxBodySize());
		if (server.getRoutes().empty())
			server.setRoutes(this->DefaultServer.getRoutes());
		if (server.getErrorPage().empty())
			throw runtime_error("\033[31m Server: " + cpp11_toString(i + 1)  + " must have an 404 error page");
	}
}

void	WebServer::printData()
{
	cout << "Default server:" << endl;
	cout << this->DefaultServer;
	for (size_t i = 0; i < this->Servers.size(); i++) {
		cout << this->Servers[i] << endl;
		cout << "---------------------------------" << endl;
	}
}

void	WebServer::CheckFiles()
{
	for (size_t i = 0; i < this->Servers.size(); i++) {
		Server &server = this->Servers[i];
		cout << bold << green << " -------- Checking files for server " << i + 1 << def << endl;
		server.CheckFiles();
	}
}

void	WebServer::run() {
	for (size_t i = 0; i < this->Servers.size(); i++) {
		Server &server = this->Servers[i];
		cout << bold << green << " -------- Starting server " << i + 1 << def << endl;
		server.initSocket();
		server.bindSocket();
		server.listenSocket();
		server.communicate();
	}
}
