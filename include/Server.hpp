/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: zouddach <zouddach@1337.student.ma>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/01/21 16:51:00 by zouddach          #+#    #+#             */
/*   Updated: 2025/02/26 19:58:10 by zouddach         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef SERVER_HPP
#define SERVER_HPP

#include "Route.hpp"
# include "Functions.hpp"
# include "File.hpp"
# include <netinet/in.h>
# include <arpa/inet.h>
# include <sys/socket.h>
# include <fcntl.h>
# include <sys/socket.h>
# include <netinet/in.h>
# include <netinet/tcp.h>


// Add OS-specific headers and definitions
#ifdef __APPLE__
    #define TCP_KEEPINTVL        0x101
    #define TCP_KEEPCNT         0x102
	#define TCP_KEEPIDLE        0x103
	
#endif

// TODO: add a vector<Server> _Friends; to the Server class, khas ykono 3ndo ga3 friends li homa servers li mcharkin m3ah fl ip && port

class Server {
	private:
		string				HostName;
		vector<string>		IndexFiles;
		string				Port;
		string				Root;
		vector<string>		ServerNames;
		long				ClientMaxBodySize; /* in bytes */
		vector<Route>		Routes;
		string				ErrorPage;
		vector<Server>		_ServerFriends;

	public:
		struct sockaddr_in	Address;
		
		int					Socket;
		Server(string hostname = "localhost", string port = "8080", string root = ".");
		~Server();
		// Server(const Server &server);
		// Server				&operator=(const Server &server);
		
		void				addRoute(const Route& route);
		void				updateAddress();
		Route				*getRouteFromUri(string uri);
		
		/* getters */
		string				getHostName() const;
		string				getPort() const;
		vector<string>		getServerNames() const;
		long				getClientMaxBodySize() const ;
		vector<Route>		getRoutes() const;
		string				getErrorPage() const;
		string				getRoot() const;
		vector<string>		getIndexFiles() const;
		vector<Server>		getFriends() const;

		/* setters */
		void				setRoot(string root);
		void				setErrorPage(string pages);
		void				setClientMaxBodySize(long size);
		void				setServerNames(vector<string> names);
		void				setRoutes(vector<Route> routes);
		void				setProperty(const string& key, string value);
		void				addFriend(Server& server);
		
		void				CheckFiles();


};

ostream			&operator<<(std::ostream &out, const Server &server);
string			trim(const string& str);
vector<string>	split(string str, char delimiter);

#endif