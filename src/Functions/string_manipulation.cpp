/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   string_manipulation.cpp                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: zouddach <zouddach@1337.student.ma>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/01/22 16:31:07 by zouddach          #+#    #+#             */
/*   Updated: 2025/01/22 17:32:42 by zouddach         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/functions.hpp"

string trim(const string& str) {
	size_t first = str.find_first_not_of(" \t");
	if (first == string::npos) return "";
	size_t last = str.find_last_not_of(" \t");
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