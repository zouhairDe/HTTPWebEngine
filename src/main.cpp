/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: zouddach <zouddach@1337.student.ma>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/01/21 18:48:44 by zouddach          #+#    #+#             */
/*   Updated: 2025/01/23 01:03:26 by zouddach         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/Global.hpp"

int main(int ac, char **av)
{
    try {
        if (ac != 2) {
            throw runtime_error("Usage: ./webserv <config_file>");
        }
		
		cout << bold << green << "Parsing config file: " << av[1] << def << endl;//anzido wahd progress bar hna anhsbo ch7al mn server 3ndna f conf w infoi nsaliw n incrementiw chi var w tb9a tzad hna
        
		WebServer webserv(av[1]);
		
		// webserv.printData();
        
        return 0;
    }
    catch (const exception& e) {
        cerr << "Error: " << e.what() << endl;
        return 1;
    }
}