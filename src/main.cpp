/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mzeggaf <mzeggaf@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/01/21 18:48:44 by zouddach          #+#    #+#             */
/*   Updated: 2025/02/05 12:13:14 by mzeggaf          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Global.hpp"

int main(int ac, char **av)
{
    try {
        if (ac != 2) {
            throw runtime_error("usage: ./webserv <config_file>");
        }
		
		cout << bold << green << "Parsing config file: " << av[1] << def << endl;
		/* anzido wahd progress bar hna 
		anhsbo ch7al mn server 3ndna f 
		conf w infoi nsaliw n incrementiw 
		chi var w tb9a tzad hna: DONE? */
        
		WebServer webserv(av[1]);
		
		// webserv.printData();
    }
    catch (const exception& e) {
        cerr << "Error: " << e.what() << endl;
        return (1);
    }
	return (0);
}
