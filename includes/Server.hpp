/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: zouddach <zouddach@1337.student.ma>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/01/21 16:51:00 by zouddach          #+#    #+#             */
/*   Updated: 2025/01/23 00:45:57 by zouddach         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef SERVER_HPP
#define SERVER_HPP

#include "Global.hpp"
#include "functions.hpp"

string trim(const string& str);
vector<string> split(string str, char delimiter);

class Server {
private:
    string HostName;
    string Port;
	string Root;
    vector<string> ServerNames;
    long ClientMaxBodySize;//in bytes
    vector<Route> Routes;
    map<string, string> ErrorPages;

public:
    Server() : ClientMaxBodySize(-1) {}
	string getHostName() const {
		return HostName;	
	}
	
	string getPort() const {
		return Port;
	}
	
	vector<string> getServerNames() const {
		return ServerNames;
	}
	
	long getClientMaxBodySize() const {
		return ClientMaxBodySize;
	}
	
	vector<Route> getRoutes() const {
		return Routes;
	}
	
	map<string, string> getErrorPages() const {
		return ErrorPages;
	}
	
	string	getRoot() const {
		return Root;
	}
	
	void	setRoot(string root) {
		Root = root;
	}
	
	void	setErrorPages(map<string, string> pages) {
		ErrorPages = pages;
	}
	
	void	setClientMaxBodySize(long size) {
		ClientMaxBodySize = size;
	}
	
	void	setServerNames(vector<string> names) {
		ServerNames = names;
	}
	
	void	setRoutes(vector<Route> routes) {
		Routes = routes;
	}

    void setProperty(const string& key, string value) {
        if (key == "host") HostName = value;
        else if (key == "port") Port = value;
        else if (key == "server_names") setServerNames(split(value, ','));
		else if (key == "root") Root = value;
        else if (key == "client_max_body_size") {
            string size = value.substr(0, value.length()-1);
            char unit = value[value.length()-1];//if unit is not a number consider it bytes
            ClientMaxBodySize = atol(size.c_str());
            if (unit == 'M') ClientMaxBodySize *= 1024 * 1024;
            else if (unit == 'K') ClientMaxBodySize *= 1024;
        }
        else if (key.find("error_page_") != string::npos) {
            string code = key.substr(10);
            ErrorPages[code] = value;
        }
    }

    void addRoute(const Route& route) {
        Routes.push_back(route);
    }
	
	friend ostream &operator<<(std::ostream &out, const Server &server)
	{
		out << "Host: " << server.getHostName() << endl;
		out << "Root: " << server.getRoot() << endl;
		out << "Port: " << server.getPort() << endl;
		out << "Server names: ";
		vector<string> names = server.getServerNames();
		for (size_t i = 0; i < names.size(); i++) {
			out << names[i];
			if (i < names.size() - 1) out << ", ";
		}
		out << endl;
		out << "Client max body size: " << server.getClientMaxBodySize() << endl;
		out << "Routes:" << endl;
		vector<Route> routes = server.getRoutes();
		for (size_t i = 0; i < routes.size(); i++) {
			Route route = routes[i];
			out << "  Route " << i + 1 << ":" << endl;
			out << "    Root: " << route.getRouteRoot() << endl;
			out << "    Index: " << route.getRouteIndex() << endl;
			out << "    Directory listing: " << (route.getRouteDirectoryListing() ? "on" : "off") << endl;
			out << "    Allowed methods: ";
			if (route.getRouteGETMethod()) out << "GET ";
			if (route.getRoutePOSTMethod()) out << "POST";
			out << endl;
			out << "    Upload store: " << route.getUploadStore() << endl;
			out << "    Client max body size: " << route.getClientMaxBodySize() << endl;
		}
		out << "Error pages:" << endl;
		map<string, string> errorPages = server.getErrorPages();
		for (map<string, string>::const_iterator it = errorPages.begin(); it != errorPages.end(); it++) {
			out << "  " << it->first << ": " << it->second << endl;
		}
		return out;
	}

};


#endif