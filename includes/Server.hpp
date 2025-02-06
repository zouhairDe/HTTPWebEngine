/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mzeggaf <mzeggaf@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/01/21 16:51:00 by zouddach          #+#    #+#             */
/*   Updated: 2025/02/06 13:35:54 by mzeggaf          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef SERVER_HPP
#define SERVER_HPP

#include "Route.hpp"
#include "functions.hpp"
#include "File.hpp"
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/socket.h>

class Server {
	private:
		string				HostName;
		string				IndexFile;
		string				Port;
		string				Root;
		vector<string>		ServerNames;
		long				ClientMaxBodySize; /* in bytes */
		vector<Route>		Routes;
		string				ErrorPage;

		struct sockaddr_in	Address;

	public:
		int					Socket;
		Server(string hostname = "localhost", string port = "8080", string root = ".");
		~Server();
		
		void				addRoute(const Route& route);
		int					initSocket(void);
		int					bindSocket(void);
		int					listenSocket(void);
		int					communicate(void);
		
		/* getters */
		string				getHostName() const;
		string				getPort() const;
		vector<string>		getServerNames() const;
		long				getClientMaxBodySize() const ;
		vector<Route>		getRoutes() const;
		string				getErrorPage() const;
		string				getRoot() const;
		string				getIndexFile() const;

		/* setters */
		void				setRoot(string root);
		void				setErrorPage(string pages);
		void				setClientMaxBodySize(long size);
		void				setServerNames(vector<string> names);
		void				setRoutes(vector<Route> routes);
		void				setProperty(const string& key, string value);
		void				CheckFiles();


};

ostream			&operator<<(std::ostream &out, const Server &server);
string			trim(const string& str);
vector<string>	split(string str, char delimiter);

#endif