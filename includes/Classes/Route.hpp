/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Route.hpp                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mzeggaf <mzeggaf@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/01/21 20:01:01 by zouddach          #+#    #+#             */
/*   Updated: 2025/02/05 12:33:24 by mzeggaf          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef ROUTE_HPP
#define ROUTE_HPP

#include "Global.hpp"

class Route {
	private:
		string	RouteName;
		string	RouteRoot;
		string	RouteIndex;
		string	UploadStore;
		bool	RouteDirectoryListing;
		bool	RouteGETMethod;
		bool	RoutePOSTMethod;
		long	ClientMaxBodySize;

	public:
		Route();
		~Route();
		
		/* getters */
		string	etRouteRoot() const;
		string	getRouteIndex() const;
		bool	getRouteDirectoryListing() const;
		bool	getRouteGETMethod() const;
		bool	getRoutePOSTMethod() const;
		string	getUploadStore() const;
		long	getClientMaxBodySize() const;
		string	getRouteName() const;

		// ma3rftch ila chi rouote ma3ndouch root/ wach n3tiwh default ("/var/www/") wla la

		/* setters */
		void setProperty(const string& key, const string& value);

};

#endif
