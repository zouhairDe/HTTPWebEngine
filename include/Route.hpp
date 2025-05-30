/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Route.hpp                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: zouddach <zouddach@student.1337.ma>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/01/21 20:01:01 by zouddach          #+#    #+#             */
/*   Updated: 2025/04/17 15:27:49 by zouddach         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef ROUTE_HPP
#define ROUTE_HPP

#include "Global.hpp"
# include "File.hpp"
# include "RequestProcessor.hpp"

class Server;
class File;
class RequestProcessor;

class Route {
	private:
		string							RouteName;
		string							RouteRoot;
		vector<string>					RouteIndexFiles;
		string							UploadStore;
		pair<string, int>				_redirectionUrl;
		bool							RouteDirectoryListing;
		bool							RouteGETMethod;
		bool							RoutePOSTMethod;
		bool							RouteDELETEMethod;
		size_t							ClientMaxBodySize;
		vector<pair<string, string> >	CGIs;

	public:
		Route();
		Route(const Route &route);
		Route			&operator=(const Route &route);
		~Route();
		
		/* getters */
		vector<string>					getRouteIndexFiles() const;
		bool							getRouteDirectoryListing() const;
		bool							getRouteGETMethod() const;
		bool							getRoutePOSTMethod() const;
		bool                            getDELETEMethod() const;
		string							getUploadStore() const;
		size_t							getClientMaxBodySize() const;
		string							getRouteName() const;
		string							getRouteRoot() const;
		void							CheckFiles(string serverRoot);
		pair<string, int>				getRedirectUrl() const;
		vector<pair<string, string> >	getCGIs() const;

		/* setters */
		void setProperty(const string& key, const string& value);
		

};

ostream &operator<<(ostream &out, const Route &route);

#endif
