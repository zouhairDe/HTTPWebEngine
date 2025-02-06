/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Route.hpp                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: zouddach <zouddach@1337.student.ma>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/01/21 20:01:01 by zouddach          #+#    #+#             */
/*   Updated: 2025/02/06 16:16:12 by zouddach         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef ROUTE_HPP
#define ROUTE_HPP

#include "Global.hpp"

class Route {
	private:
		string	RouteName;
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
		string	getRouteIndex() const;
		bool	getRouteDirectoryListing() const;
		bool	getRouteGETMethod() const;
		bool	getRoutePOSTMethod() const;
		string	getUploadStore() const;
		long	getClientMaxBodySize() const;
		string	getRouteName() const;
		void	CheckFiles() const;

		// ma3rftch ila chi rouote ma3ndouch root/ wach n3tiwh default ("/var/www/") wla la

		/* setters */
		void setProperty(const string& key, const string& value);

};

#endif
