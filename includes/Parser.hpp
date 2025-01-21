/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Parser.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: zouddach <zouddach@1337.student.ma>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/01/21 16:41:06 by zouddach          #+#    #+#             */
/*   Updated: 2025/01/21 23:16:25 by zouddach         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef PARSER_HPP
#define PARSER_HPP

#include "./Global.hpp"

class ConfigParser {
	private:
		string ConfigFileName;
		string ConfigFilePath;
		
		string trim(const string& str) {
			size_t first = str.find_first_not_of(" \t");
			if (first == string::npos) return "";
			size_t last = str.find_last_not_of(" \t");
			return str.substr(first, (last - first + 1));
		}

		bool isServerBlock(const string& line) {
			return line.find("[server,") != string::npos || line == "[server]";
		}

		bool isRouteBlock(const string& line) {
			if (line.find("[route") == string::npos)
				return false;
			//check if its [route] or [route "name"]
			if (line.find("[route]") != string::npos)//if rout consider it later as [route, "/"]
				return true;
			
			if (line.find("[route ") != string::npos && line.find("\"]") != string::npos)
				return true;
				
			return false;
		}
		
		bool isServerEndBlock(const string& line) {
			return line[0] == '[' && line[line.length()-1] == ']' 
				&& line[1] == '\"'
				&& line.find("\"]") != string::npos;
		}

		bool isRouteEndBlock(const string& line, const string& routeName) {
			line = trim(line);
			line = line.replace(" ", "");
			line = "[" + line + "]";
			if (line == routeName)
				return true;
			return false;
		}

		pair<string, string> parseKeyValue(const string& line) {
			size_t pos = line.find("=");
			if (pos == string::npos) //mimknch tkhilh khawi... todo later
				throw runtime_error("Invalid configuration line: " + line);
				
			string key = trim(line.substr(0, pos));
			string value = trim(line.substr(pos + 1));
			key = key.replace(" ", "");
			value = value.replace(" ", "");
			return make_pair(key, value);
		}

		void parseServerBlock(ifstream& file, Server& server) {
			string line; 
			while (getline(file, line)) {
				line = trim(line);
				if (line.empty() || line[0] == '#') continue;
				
				if (isServerEndBlock(line)) break;
				if (isRouteBlock(line)) {
					Route route;
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
		
		vector<string> split(string str, char delimiter) {
			vector<string> internal;
			stringstream ss(str);
			string tok;
			
			while(getline(ss, tok, delimiter)) {
				internal.push_back(tok);
			}
			return internal;
		}

		void parseRouteBlock(ifstream& file, Route& route) {
			string line;
			string routeName = "";
			getline(file, line);
			line = trim(line);
			line = line.replace(" ", "");
			if ((line.find("[route,") != string::npos && line.find("\"]") != string::npos) || line.find("[route]") != string::npos) {
				vector<string> parts = split(line, ',');
				if (parts.size() > 2)
					throw runtime_error("Invalid route block: " + line);
				if (parts.size() == 2)
					routeName = parts[1].substr(0, parts[1].length()-1);
				else
					routeName = "/";
				route.setProperty("name", routeName);
			}
			while (getline(file, line)) {
				line = trim(line);
				if (line.empty() || line[0] == '#') continue;
				
				if (isRouteEndBlock(line, routeName)) break;

				if (!line.empty()) {
					pair<string, string> kv = parseKeyValue(line);
					route.setProperty(kv.first, kv.second);
				}
			}
		}
		
		bool fileExists(const string& filename) {
			ifstream file(filename.c_str());
			return file.good();
		}

	public:
		ConfigParser(const char* filename) {
			this->ConfigFileName = filename;
			this->ConfigFilePath = string(filename);
			if (!fileExists(ConfigFilePath))
				throw runtime_error("Configuration file does not exist");
		}
		
		string getFileName() const {
			return ConfigFileName;
		}
		
		string getFilePath() const {
			return ConfigFilePath;
		}
		vector<Server> parseConfig(const string& filename) {
			vector<Server> servers;
			ifstream file(filename.c_str());
			
			if (!file.is_open())
				throw runtime_error("Cannot open configuration file: " + filename);

			string line;
			while (getline(file, line)) {
				line = trim(line);
				if (line.empty() || line[0] == '#') continue;

				if (isServerBlock(line)) {
					Server server;
					parseServerBlock(file, server);
					servers.push_back(server);
				}
			}

			file.close();
			return servers;
		}
		
		void printData()
		{
			vector<Server> servers = parseConfig(ConfigFilePath);
			for (size_t i = 0; i < servers.size(); i++) {
				Server server = servers[i];
				cout << "Server " << i + 1 << ":" << endl;
				cout << "Host: " << server.getHostName() << endl;
				cout << "Port: " << server.getPort() << endl;
				cout << "Server names: ";
				vector<string> names = server.getServerNames();
				for (size_t j = 0; j < names.size(); j++) {
					cout << names[j];
					if (j < names.size() - 1) cout << ", ";
				}
				cout << endl;
				cout << "Client max body size: " << server.getClientMaxBodySize() << endl;
				cout << "Routes:" << endl;
				vector<Route> routes = server.getRoutes();
				for (size_t j = 0; j < routes.size(); j++) {
					Route route = routes[j];
					cout << "  Route " << j + 1 << ":" << endl;
					cout << "    Root: " << route.getRouteRoot() << endl;
					cout << "    Index: " << route.getRouteIndex() << endl;
					cout << "    Directory listing: " << (route.getRouteDirectoryListing() ? "on" : "off") << endl;
					cout << "    Allowed methods: ";
					if (route.getRouteGETMethod()) cout << "GET ";
					if (route.getRoutePOSTMethod()) cout << "POST";
					cout << endl;
					cout << "    Upload store: " << route.getUploadStore() << endl;
					cout << "    Client max body size: " << route.getClientMaxBodySize() << endl;
				}
				cout << "Error pages:" << endl;
				map<string, string> errorPages = server.getErrorPages();
				for (map<string, string>::iterator it = errorPages.begin(); it != errorPages.end(); it++) {
					cout << "  " << it->first << ": " << it->second << endl;
				}
			}
		}
};

#endif