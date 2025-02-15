/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   WebServer.hpp                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: zouddach <zouddach@1337.student.ma>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/01/21 19:49:51 by zouddach          #+#    #+#             */
/*   Updated: 2025/02/15 17:03:31 by zouddach         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef WEBSERVER_HPP
# define WEBSERVER_HPP

# include "ConfigParser.hpp"

class WebServer {
	private:
		Server	DefaultServer;
		vector<Server>	Servers;
		vector<string>	RunningServers;/* Host:Port */
	public:
		WebServer(char* filename);
		~WebServer();
		
		void	changeEmptyValues();
		void	printData();
		void	CheckFiles();
		void	run();
		int		communicate();
		Server* getServerBySocket(int socket_fd);
		int 	handleNewConnection(Server& server);
		int 	handleClientData(int client_fd, Server& server);
		Server* findServerByHost(const RequestProccessor& req, Server& defaultServer);
    	bool	isMatchingServer(const vector<string>& serverNames, const string& requestHost);
			
};

#endif