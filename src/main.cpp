/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: zouddach <zouddach@student.1337.ma>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/01/21 18:48:44 by zouddach          #+#    #+#             */
/*   Updated: 2025/04/17 15:43:53 by zouddach         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

# include "Global.hpp"
#include "WebServer.hpp"
#include "Server.hpp"

void f()
{
	system("lsof -c webserv");
}

int main(int ac, char **av)
{
	atexit(f);
    signal(SIGPIPE, SIG_IGN);
    try {
        if (ac != 2) {
            throw runtime_error("\033[31m usage: ./webserv <config_file>");
        }
        string filename = av[1];
        for (size_t i = filename.length() - CONF_EXTENTION.length(); i < filename.length(); i++) {
            if (filename[i] != CONF_EXTENTION[i - (filename.length() - CONF_EXTENTION.length())]) {
                throw runtime_error("\033[31m Error: config file must end with .conf");
            }
        }

        ifstream file(av[1]);
        if (!file.is_open()) {
            throw runtime_error("\033[31m Error: config file does not exist");
        }
		
		cout << bold << green << "Parsing config file: " << av[1] << def << endl;
		WebServer webserv(av[1]);
		webserv.CheckFiles();
		webserv.run();
    }
    catch (const exception& e) {
        cerr << "Error: " << e.what() << endl;
        return (1);
    }
	return (0);
}
