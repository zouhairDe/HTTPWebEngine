/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   WebServer.hpp                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: zouddach <zouddach@1337.student.ma>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/01/21 19:49:51 by zouddach          #+#    #+#             */
/*   Updated: 2025/02/26 20:02:29 by zouddach         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef WEBSERVER_HPP
# define WEBSERVER_HPP

# include "ConfigParser.hpp"

class WebServer {
	private:
		vector<Server>	Servers;
		vector<int>		RunningSockets;
	public:
		WebServer(char* filename);
		~WebServer();
		
		void	changeEmptyValues();
		void	printData();
		void	CheckFiles();
		void	run();
		int 	handleNewConnection(int server_fd, int epoll_fd);
};

#endif