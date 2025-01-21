/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: zouddach <zouddach@1337.student.ma>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/01/21 16:51:00 by zouddach          #+#    #+#             */
/*   Updated: 2025/01/21 20:29:32 by zouddach         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef SERVER_HPP
#define SERVER_HPP

#include "Global.hpp"

class Server {
private:
    string HostName;
    string Port;
    vector<string> ServerNames;
    long ClientMaxBodySize;
    vector<Route> Routes;
    map<string, string> ErrorPages;

public:
    Server() : ClientMaxBodySize(0) {}
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
	
	

    void setProperty(const string& key, const string& value) {
        if (key == "host") HostName = value;
        else if (key == "port") Port = value;
        else if (key == "server_names") ServerNames.push_back(value);
        else if (key == "client_max_body_size") {
            string size = value.substr(0, value.length()-1);
            char unit = value[value.length()-1];
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
};

#endif