/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: zouddach <zouddach@student.1337.ma>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/01/21 16:51:00 by zouddach          #+#    #+#             */
/*   Updated: 2025/04/17 15:27:31 by zouddach         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef SERVER_HPP
#define SERVER_HPP

# include "Global.hpp"
# include "Route.hpp"
# include "Functions.hpp"
# include "File.hpp"
# include <netinet/in.h>
# include <arpa/inet.h>
# include <sys/socket.h>
# include <fcntl.h>
# include <sys/socket.h>
# include <netinet/tcp.h>
# include <sys/types.h>
# include <sys/epoll.h>
# include <cstring>
# include <string>
# include <cstddef>

class Route;

class Server {
	private:
		string								HostName;
		vector<string>						IndexFiles;
		string								Port;
		string								Root;
		vector<string>						ServerNames;
		long								ClientMaxBodySize; /* in bytes */
		string								ErrorPage;
		pair<string, int>					_redirectionUrl;
		vector<Route>						_Routes;
		vector<Server>						_ServerFriends;

	public:
		struct sockaddr_in	Address;
		
		int					Socket;
		Server(string hostname = "localhost", string port = "8080", string root = ".");
		~Server();
		
		Server &operator=(const Server &server);
		// Server(const Server &server);
		// Server				&operator=(const Server &server);
		
		int					init(int epoll_fd);
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
		pair<string, int>	getRedirectUrl() const;
		bool				isRouteExist(string route);

		/* setters */
		void				setRoot(string root);
		void				setErrorPage(string pages);
		void				setClientMaxBodySize(long size);
		void				setServerNames(vector<string> names);
		void				setRoutes(vector<Route> routes);
		void				setProperty(const string& key, string value);
		void				addFriend(Server& server);
		bool 				serverHasRootRoute() const;
		
		void				CheckFiles();


};

ostream			&operator<<(std::ostream &out, const Server &server);
string			trim(const string& str);
vector<string>	split(string str, char delimiter);

#endif