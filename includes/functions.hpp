/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   functions.hpp                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: zouddach <zouddach@1337.student.ma>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/01/22 16:31:36 by zouddach          #+#    #+#             */
/*   Updated: 2025/01/22 17:17:55 by zouddach         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef FUNCTIONS_HPP
#define FUNCTIONS_HPP

# include "Global.hpp"

string trim(const string& str);
vector<string> split(string str, char delimiter);
string cpp11_replace(string  &str, string toReplace, string replacement);
pair<string, string> parseKeyValue(const string& line);

#endif