#include "ConfigParser.hpp"

bool ConfigParser::isServerEndBlock(const string& line) {
	return line[0] == '[' && line[line.length()-1] == ']' 
		&& line[1] == '\"'
		&& line.find("\"]") != string::npos;
}

void ConfigParser::parseServerBlock(ifstream& file, Server& server) {
	string line; 
	while (getline(file, line)) {
		line = trim(line);
		if (line.empty() || line[0] == '#') continue ;

		if (isServerEndBlock(line)) {
			server.updateAddress();
			break;
		};
		string routeName = "";
		if (isRouteBlock(line, routeName)) {
			Route route;
			if (route.getRouteName().empty())
				route.setProperty("name", routeName);
			parseRouteBlock(file, route);
			server.addRoute(route);
			continue ;
		}

		if (!line.empty()) {
			pair<string, string> kv = parseKeyValue(line);
			server.setProperty(kv.first, kv.second);
		}
	}
}

bool ConfigParser::isRouteEndBlock(string line, const string& routeName) {

	line = trim(line);
	line = cpp11_replace(line, " ", "");
	line = cpp11_replace(line, "[", "");
	line = cpp11_replace(line, "]", "");
	if (line == routeName)
		return true;
	return false;
}

void ConfigParser::parseRouteBlock(ifstream& file, Route& route) {
	string line;
	while (getline(file, line)) {
		line = trim(line);
		if (line.empty() || line[0] == '#') continue;

		if (isRouteEndBlock(line, route.getRouteName())) break;

		if (!line.empty()) {
			pair<string, string> kv = parseKeyValue(line);
			route.setProperty(kv.first, kv.second);
		}
	}
}

bool ConfigParser::fileExists(const string& filename) {
	ifstream file(filename.c_str());
	return file.good();
}

void ConfigParser::displayProgressBar(int current, int total) {
	const int barWidth = 60;
	float progress = (float)current / total;
	int pos = barWidth * progress;
	usleep(50000); 

	cout << "\r[";
	for (int i = 0; i < barWidth; ++i) {
		if (i < pos) cout << "=";
		else if (i == pos) cout << ">";
		else cout << " ";
	}
	cout << "] " << int(progress * 100.0) << "%";
	cout.flush();
}

ConfigParser::ConfigParser(const char* filename) {
	this->ConfigFileName = filename;
	this->ConfigFilePath = string(filename);
	this->lineCount = 0;
	this->serverCount = 0;
	struct stat s;
	cout << bold ;
	if (stat(filename, &s) == -1)
		throw runtime_error("\033[31m Cannot open configuration file: " + ConfigFileName);
	if (!fileExists(filename))
		throw runtime_error("\033[31m Configuration file does not exist: " + ConfigFileName);
	if (s.st_size == 0)
		throw runtime_error("\033[31m Configuration file is empty: " + ConfigFileName);
	if (s.st_mode & S_IFDIR)
		throw runtime_error("\033[31m Configuration file is a directory: " + ConfigFileName);
	if (s.st_mode & S_IFREG)
	{
		if (s.st_size == 0)
			throw runtime_error("\033[31m Configuration file is empty: " + ConfigFileName);
	}
	else
		throw runtime_error("\033[31m Configuration file is not a regular file: " + ConfigFileName);
	cout << def;
}

ConfigParser::~ConfigParser() { };

bool ConfigParser::isServerBlock(const string& line) {
	return line.find("[server,") != string::npos || line == "[server]";
}

bool ConfigParser::isRouteBlock(string& line, string &routeName) {
	line = trim(line);
	line = cpp11_replace(line, " ", "");
	if ((line.find("[route,") != string::npos && line.find("\"]") != string::npos) || line == "[route]") {
		vector<string> parts = split(line, ',');
		if (parts.size() > 2)
			throw runtime_error("\033[31m Invalid route block: " + line);
		if (parts.size() == 2)
			routeName = parts[1].substr(0, parts[1].length()-1);
		else
			routeName = "\"/\"";
		return true;
	}
	if (line.find("[route") == string::npos)
		return false;
	return false;
}

string ConfigParser::getFileName() const {
	return ConfigFileName;
}

string ConfigParser::getFilePath() const {
	return ConfigFilePath;
}

bool	ConfigParser::addServerToFriends(Server& server, vector<Server>& servers) {
	for (size_t i = 0; i < servers.size(); i++) {
		if (servers[i].getHostName() == server.getHostName() && servers[i].getPort() == server.getPort()) {
			servers[i].addFriend(server);
			return true;
		}
	}
	return false;
}

vector<Server> ConfigParser::parseConfig(const string& filename) {
	vector<Server> servers;
	ifstream file(filename.c_str());

	if (!file.is_open())
		throw runtime_error("\033[31m Cannot open configuration file: " + filename);

	string line;
	while (getline(file, line)) {
		line = trim(line);
		if (line.empty() || line[0] == '#') continue;

		if (isServerBlock(line)) {
			Server server;
			parseServerBlock(file, server);
			if (!addServerToFriends(server, servers))
				servers.push_back(server);
			displayProgressBar(servers.size(), serverCount);
		}
	}

	file.close();
	return servers;
}

void ConfigParser::setServerCount(int count) {
	serverCount = count;
}