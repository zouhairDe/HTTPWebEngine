/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   WebServer.hpp                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mzeggaf <mzeggaf@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/01/21 19:49:51 by zouddach          #+#    #+#             */
/*   Updated: 2025/02/05 13:30:01 by mzeggaf          ###   ########.fr       */
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
		
		//check if any empty value was found in default server return runtime_error
		//check if any value is empty in Servers replace it by the default server value
		
		void	changeEmptyValues();
		void	printData();
		
};

#endif