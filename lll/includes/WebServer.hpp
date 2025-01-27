/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   WebServer.hpp                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: zouddach <zouddach@1337.student.ma>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/01/21 19:49:51 by zouddach          #+#    #+#             */
/*   Updated: 2025/01/22 20:20:18 by zouddach         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef WEBSERVER_HPP
# define WEBSERVER_HPP

# include "./Global.hpp"


class WebServer {
	private:
		Server	DefaultServer;
		vector<Server>	Servers;
	public:
		WebServer(char* filename) {
			//khas ncheciw b3da wach kayn error line fl config file
			ConfigParser config(filename);
			ifstream file(filename);
			string line;
			int serverCount = 0;
			while (getline(file, line)) {
				line = trim(line);
				if (line.empty() || line[0] == '#') continue;
				if (config.isServerBlock(line)) {
					serverCount++;
				}
			}
			cerr << "Server count: " << serverCount << endl;
			file.close();
			config.setServerCount(serverCount);

			// DefaultServer = config.parseConfig("config/default.conf")[0];
			Servers = config.parseConfig(string(filename));
			changeEmptyValues();
		}
		
		//check if any empty value was found in default server return runtime_error
		//check if any value is empty in Servers replace it by the default server value
		
		void	changeEmptyValues()
		{
			// cerr << "Changing Empty values for servers" << endl;
			for (size_t i = 0; i < this->Servers.size(); i++)
			{
				// cerr << "Changing For Server " << i + 1 << endl;
				Server &server = this->Servers[i];
				if (server.getHostName().empty())
					server.setProperty("host", this->DefaultServer.getHostName());
				if (server.getPort().empty())
					server.setProperty("port", this->DefaultServer.getPort());
				if (server.getServerNames().empty())
					server.setServerNames(this->DefaultServer.getServerNames());
				if (server.getClientMaxBodySize() == -1)
					server.setClientMaxBodySize(this->DefaultServer.getClientMaxBodySize());
				if (server.getRoutes().empty())
					server.setRoutes(this->DefaultServer.getRoutes());
				if (server.getErrorPages().empty())
					server.setErrorPages(this->DefaultServer.getErrorPages());
				
			}
		}
		
		void printData()
		{
			cout << "Default server:" << endl;
			cout << this->DefaultServer;
			for (size_t i = 0; i < this->Servers.size(); i++) {
				cout << this->Servers[i];
			}
		}
		
};

#endif