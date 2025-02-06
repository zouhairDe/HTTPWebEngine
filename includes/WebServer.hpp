/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   WebServer.hpp                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mzeggaf <mzeggaf@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/01/21 19:49:51 by zouddach          #+#    #+#             */
/*   Updated: 2025/02/06 13:44:48 by mzeggaf          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef WEBSERVER_HPP
# define WEBSERVER_HPP

# include "ConfigParser.hpp"

class WebServer {
	private:
		Server	DefaultServer;
		vector<Server>	Servers;
	public:
		WebServer(char* filename);
		~WebServer();
		
		void	changeEmptyValues();
		void	printData();
		void	CheckFiles();
		void	run();
		
};

#endif