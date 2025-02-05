/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mzeggaf <mzeggaf@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/01/21 16:51:00 by zouddach          #+#    #+#             */
/*   Updated: 2025/02/05 12:48:33 by mzeggaf          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef SERVER_HPP
#define SERVER_HPP

#include "Global.hpp"
#include "functions.hpp"

string			trim(const string& str);
vector<string>	split(string str, char delimiter);

class Server {
	private:
		string				HostName;
		string				Port;
		string				Root;
		vector<string>		ServerNames;
		long				ClientMaxBodySize; /* in bytes */
		vector<Route>		Routes;
		map<string, string>	ErrorPages;

	public:
		Server();
		~Server();
		
		void				addRoute(const Route& route);
		
		/* getters */
		string				getHostName() const;
		string				getPort() const;
		vector<string>		getServerNames() const;
		long				getClientMaxBodySize() const ;
		vector<Route>		getRoutes() const;
		map<string, string>	getErrorPages();
		string				getRoot() const;

		/* setters */
		void				setRoot(string root);
		void				setErrorPages(map<string, string> pages);
		void				setClientMaxBodySize(long size);
		void				setServerNames(vector<string> names);
		void				setRoutes(vector<Route> routes);
		void				setProperty(const string& key, string value);

		friend ostream &operator<<(std::ostream &out, const Server &server);

};

#endif