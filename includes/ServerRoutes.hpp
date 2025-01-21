/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ServerRoutes.hpp                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: zouddach <zouddach@1337.student.ma>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/01/21 20:01:01 by zouddach          #+#    #+#             */
/*   Updated: 2025/01/21 22:55:07 by zouddach         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef SERVER_ROUTES_HPP
#define SERVER_ROUTES_HPP

#include "Global.hpp"

class Route {
private:
	string RouteName;
    string RouteRoot;
    string RouteIndex;
    bool RouteDirectoryListing;
    bool RouteGETMethod;
    bool RoutePOSTMethod;
    string UploadStore;
    long ClientMaxBodySize;

public:
    Route() : RouteDirectoryListing(false), RouteGETMethod(false), 
              RoutePOSTMethod(false), ClientMaxBodySize(0) {}
			  
	string getRouteRoot() const {
		return RouteRoot;
	}
	
	string getRouteIndex() const {
		return RouteIndex;
	}
	
	bool getRouteDirectoryListing() const {
		return RouteDirectoryListing;
	}
	
	bool getRouteGETMethod() const {
		return RouteGETMethod;
	}
	
	bool getRoutePOSTMethod() const {
		return RoutePOSTMethod;
	}
	
	string getUploadStore() const {
		return UploadStore;
	}
	
	long getClientMaxBodySize() const {
		return ClientMaxBodySize;
	}
	
	string getRouteName() const {
		return RouteName;
	}
	
	

    void setProperty(const string& key, const string& value) {
        if (key == "root") RouteRoot = value;
		else if (key == "name") RouteName = value;
        else if (key == "index") RouteIndex = value;
        else if (key == "directory_listing") 
            RouteDirectoryListing = (value == "on");
        else if (key == "allowed_methods") {
            RouteGETMethod = (value.find("GET") != string::npos);
            RoutePOSTMethod = (value.find("POST") != string::npos);
        }
        else if (key == "upload_store") UploadStore = value;
        else if (key == "client_max_body_size") {
            string size = value.substr(0, value.length()-1);
            char unit = value[value.length()-1];
            ClientMaxBodySize = atol(size.c_str());
            if (unit == 'M') ClientMaxBodySize *= 1024 * 1024;
            else if (unit == 'K') ClientMaxBodySize *= 1024;
        }
    }
};

#endif