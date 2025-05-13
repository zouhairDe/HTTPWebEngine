/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   string_manipulation.cpp                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: zouddach <zouddach@1337.student.ma>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/01/22 16:31:07 by zouddach          #+#    #+#             */
/*   Updated: 2025/02/15 17:00:48 by zouddach         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Functions.hpp"

string trim(const string& str) {
    // Find first non-whitespace character (including newlines)
    size_t first = str.find_first_not_of(" \t\r\n");
    if (first == string::npos) return "";
    
    // Find last non-whitespace character (including newlines) 
    size_t last = str.find_last_not_of(" \t\r\n");
    
    // Return the trimmed substring
    return str.substr(first, (last - first + 1));
}

vector<string> split(string str, char delimiter) {
	vector<string> internal;
	stringstream ss(str);
	string tok;
	
	while(getline(ss, tok, delimiter)) {
		internal.push_back(tok);
	}
	return internal;
}

vector<string> splitByString(string str, string delimiter) {
    vector<string> tokens;
    size_t pos = 0;
    size_t prev = 0;
    
    while ((pos = str.find(delimiter, prev)) != string::npos) {
        tokens.push_back(str.substr(prev, pos - prev));
        prev = pos + delimiter.length();
    }
    tokens.push_back(str.substr(prev));
    return tokens;
}

string cpp11_replace(string  &str, string toReplace, string replacement)
{
	if (str.empty())
		return str;
		
	size_t pos = 0;
	std::string result;
	size_t lastPos = 0;
	
	while ((pos = str.find(toReplace, pos)) != std::string::npos) {
		result.append(str.substr(lastPos, pos - lastPos));
		result.append(replacement);
		
		pos += toReplace.length();
		lastPos = pos;
	}
	
	if (lastPos < str.length()) {
		result.append(str.substr(lastPos));
	}
	
	str = result;
	return str;
}

string cpp11_toString(int n)
{
	stringstream ss;
	ss << n;
	return ss.str();
}