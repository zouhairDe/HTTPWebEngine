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
		string			RouteName;
		vector<string>	RouteIndexFiles;
		string			UploadStore;
		bool			RouteDirectoryListing;
		bool			RouteGETMethod;
		bool			RoutePOSTMethod;
		long			ClientMaxBodySize;

	public:
		Route();
		Route(const Route &route);
		Route			&operator=(const Route &route);
		~Route();
		
		/* getters */
		vector<string>	getRouteIndexFiles() const;
		bool			getRouteDirectoryListing() const;
		bool			getRouteGETMethod() const;
		bool			getRoutePOSTMethod() const;
		string			getUploadStore() const;
		long			getClientMaxBodySize() const;
		string			getRouteName() const;
		void			CheckFiles() const;

		// ma3rftch ila chi rouote ma3ndouch root/ wach n3tiwh default ("/var/www/") wla la

		/* setters */
		void setProperty(const string& key, const string& value);
		
		/* Methods */
		File	*getGETResponse(RequestProcessor req, string root, string fileFromUri) const;
		string	createDirectoryListing(const string& path) const;
		File	*handleDirectory(const string& path) const;
		File	*handleFile(string path) const;
		

};

ostream &operator<<(ostream &out, const Route &route);

#endif
