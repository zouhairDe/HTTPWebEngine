/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ConfigParser.hpp                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mzeggaf <mzeggaf@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/01/21 16:41:06 by zouddach          #+#    #+#             */
/*   Updated: 2025/02/05 13:24:12 by mzeggaf          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef CONFIGPARSER_HPP
#define CONFIGPARSER_HPP

#include "Server.hpp"

class ConfigParser {
    private:
        string	ConfigFileName;
        string	ConfigFilePath;
        int 	lineCount;
        int 	serverCount;
	
	public:

        ConfigParser(const char* filename);
		~ConfigParser();
        
		bool			isRouteEndBlock(string line, const string& routeName);
        void			parseServerBlock(ifstream& file, Server& server);
        void			parseRouteBlock(ifstream& file, Route& route);
        bool			isRouteBlock(string& line, string &routeName);
        void			displayProgressBar(int current, int total);
        bool			isServerEndBlock(const string& line);
        vector<Server>	parseConfig(const string& filename);
        bool			fileExists(const string& filename);
		bool			isServerBlock(const string& line);

		/* getters */
        string			getFileName() const;
        string			getFilePath() const;

		/* setters */
        void			setServerCount(int count);
};

string					trim(const string& str);
vector<string>			split(string str, char delimiter);
pair<string, string>	parseKeyValue(const string& line);

#endif