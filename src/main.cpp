/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: zouddach <zouddach@1337.student.ma>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/01/21 18:48:44 by zouddach          #+#    #+#             */
/*   Updated: 2025/02/05 20:04:27 by zouddach         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "WebServer.hpp"
#include "Server.hpp"

int main(int ac, char **av)
{
    try {
        if (ac != 2) {
            throw runtime_error("\033[31m usage: ./webserv <config_file>");
        }
		
		cout << bold << green << "Parsing config file: " << av[1] << def << endl;
		WebServer webserv(av[1]);
		// webserv.printData();
		webserv.CheckFiles();
		// webserv.run();
    }
    catch (const exception& e) {
        cerr << "Error: " << e.what() << endl;
        return (1);
    }
	return (0);
}
