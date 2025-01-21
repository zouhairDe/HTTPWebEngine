/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: zouddach <zouddach@1337.student.ma>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/01/21 18:48:44 by zouddach          #+#    #+#             */
/*   Updated: 2025/01/21 20:27:38 by zouddach         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/Global.hpp"

int main(int ac, char **av)
{
    try {
        if (ac != 2) {
            throw runtime_error("Usage: ./webserv <config_file>");
        }
        
        ConfigParser config(av[1]);
		cout << "Config file: " << config.getFileName() << endl;
		cout << "Config path: " << config.getFilePath() << endl;
		
		config.printData();
        
        return 0;
    }
    catch (const exception& e) {
        cerr << "Error: " << e.what() << endl;
        return 1;
    }
}