/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   functions.hpp                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: zouddach <zouddach@1337.student.ma>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/01/22 16:31:36 by zouddach          #+#    #+#             */
/*   Updated: 2025/02/13 18:14:01 by zouddach         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef FUNCTIONS_HPP
#define FUNCTIONS_HPP

# include "Global.hpp"
# include "File.hpp"

string					cpp11_toString(int n);
string					cpp11_replace(string  &str, string toReplace, string replacement);
vector<string>			split(string str, char delimiter);
string					trim(const string& str);
pair<string, string>	parseKeyValue(const string& line);
string					generateHttpHeaders(const File& file, int status_code);

#endif