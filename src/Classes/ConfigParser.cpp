
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
		if (line.empty() || line[0] == '#') continue;
		
		if (isServerEndBlock(line)) break;
		string routeName = "";
		if (isRouteBlock(line, routeName)) {
			Route route;
			if (route.getRouteName().empty())
				route.setProperty("name", routeName);
			parseRouteBlock(file, route);
			server.addRoute(route);
			continue;
		}

		if (!line.empty()) {
			pair<string, string> kv = parseKeyValue(line);
			server.setProperty(kv.first, kv.second);
		}
	}
}

bool ConfigParser::isRouteEndBlock(string line, const string& routeName) {
	// cerr << "Checking if route end block, line: " << line << endl;
	// cerr << "Route name: " << routeName << endl;
	line = trim(line);
	line = cpp11_replace(line, " ", "");
	line = cpp11_replace(line, "[", "");
	line = cpp11_replace(line, "]", "");
	// cerr << "New line : " << line << endl;
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

void ConfigParser::displayProgressBar(int current, int total, bool isDefault) {
	if (isDefault) return;
	const int barWidth = 60;
	float progress = (float)current / total;
	int pos = barWidth * progress;
	usleep(50000); // 50ms delay bach tban lbar w hia kat7rk hhhhhhh

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
	if (!fileExists(ConfigFilePath))
		throw runtime_error("\033[31m Configuration file does not exist");
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
vector<Server> ConfigParser::parseConfig(const string& filename, bool isDefault) {
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
			servers.push_back(server);
			displayProgressBar(servers.size(), serverCount, isDefault);
		}
	}

	file.close();
	return servers;
}

void ConfigParser::setServerCount(int count) {
	serverCount = count;
}
