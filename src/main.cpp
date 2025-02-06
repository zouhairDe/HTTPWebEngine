/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mzeggaf <mzeggaf@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/01/21 18:48:44 by zouddach          #+#    #+#             */
/*   Updated: 2025/02/06 16:07:09 by mzeggaf          ###   ########.fr       */
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
		// webserv.CheckFiles();
    }
    catch (const exception& e) {
        cerr << "Error: " << e.what() << endl;
        return (1);
    }
	return (0);
}
