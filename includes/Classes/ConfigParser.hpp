/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ConfigParser.hpp                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mzeggaf <mzeggaf@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/01/21 16:41:06 by zouddach          #+#    #+#             */
/*   Updated: 2025/02/05 12:33:16 by mzeggaf          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef CONFIGPARSER_HPP
#define CONFIGPARSER_HPP

#include "Global.hpp"

class ConfigParser {
    private:
        string	ConfigFileName;
        string	ConfigFilePath;
        int 	lineCount;
        int 	serverCount;
	
	public:

        ConfigParser(const char* filename);
		~ConfigParser();
        
        bool			isServerEndBlock(const string& line);
        void			parseServerBlock(ifstream& file, Server& server);
		bool			isRouteEndBlock(string line, const string& routeName);
        void			parseRouteBlock(ifstream& file, Route& route);
        bool			fileExists(const string& filename);
        void			displayProgressBar(int current, int total);
		bool			isServerBlock(const string& line);
        bool			isRouteBlock(string& line, string &routeName);
        vector<Server>	parseConfig(const string& filename);

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