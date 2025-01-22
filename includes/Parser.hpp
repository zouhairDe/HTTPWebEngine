/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Parser.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: zouddach <zouddach@1337.student.ma>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/01/21 16:41:06 by zouddach          #+#    #+#             */
/*   Updated: 2025/01/22 17:50:09 by zouddach         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef PARSER_HPP
#define PARSER_HPP

#include "./Global.hpp"

string trim(const string& str);
vector<string> split(string str, char delimiter);
pair<string, string> parseKeyValue(const string& line);

class ConfigParser {
	private:
		string ConfigFileName;
		string ConfigFilePath;

		bool isServerBlock(const string& line) {
			return line.find("[server,") != string::npos || line == "[server]";
		}

		bool isRouteBlock(string& line, string &routeName) {
			line = trim(line);
			line = cpp11_replace(line, " ", "");
			// cerr << "Route block: " << line << endl;
			if ((line.find("[route,") != string::npos && line.find("\"]") != string::npos) || line == "[route]") {
				// cerr << "Route block found" << endl;
				vector<string> parts = split(line, ',');
				// for (size_t i = 0; i < parts.size(); i++)
				// 	cerr << "Part " << i << ": " << parts[i] << endl;
				if (parts.size() > 2)
					throw runtime_error("Invalid route block: " + line);
				if (parts.size() == 2)
					routeName = parts[1].substr(0, parts[1].length()-1);
				else
					routeName = "\"/\"";
				// cerr << "Route name: " << routeName << endl;
				return true;
			}
			if (line.find("[route") == string::npos)
				return false;
			return false;
		}
		
		bool isServerEndBlock(const string& line) {
			return line[0] == '[' && line[line.length()-1] == ']' 
				&& line[1] == '\"'
				&& line.find("\"]") != string::npos;
		}

		bool isRouteEndBlock(string line, const string& routeName) {
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

		void parseServerBlock(ifstream& file, Server& server) {
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
					// cerr << "Parsing route block" << endl;
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

		void parseRouteBlock(ifstream& file, Route& route) {
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
};

#endif