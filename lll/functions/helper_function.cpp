/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   helper_functions.cpp                               :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: zouddach <zouddach@1337.student.ma>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/01/22 17:15:52 by zouddach          #+#    #+#             */
/*   Updated: 2025/01/22 17:16:03 by zouddach         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

# include "../includes/functions.hpp"

pair<string, string> parseKeyValue(const string& line) {
	size_t pos = line.find("=");
	if (pos == string::npos) //mimknch tkhilh khawi... todo later
		throw runtime_error("Invalid configuration line: " + line);
		
	string key = trim(line.substr(0, pos));
	string value = trim(line.substr(pos + 1));
	key = cpp11_replace(key, " ", "");
	value = cpp11_replace(value, " ", "");
	return make_pair(key, value);
}