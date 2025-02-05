/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   WebServer.hpp                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: zouddach <zouddach@1337.student.ma>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/01/21 19:49:51 by zouddach          #+#    #+#             */
/*   Updated: 2025/02/05 16:36:34 by zouddach         ###   ########.fr       */
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
		
};

#endif