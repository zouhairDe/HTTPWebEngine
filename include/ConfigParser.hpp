/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ConfigParser.hpp                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: zouddach <zouddach@1337.student.ma>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/01/21 16:41:06 by zouddach          #+#    #+#             */
/*   Updated: 2025/02/26 20:00:20 by zouddach         ###   ########.fr       */
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
		bool			addServerToFriends(Server& server, vector<Server>& servers);

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