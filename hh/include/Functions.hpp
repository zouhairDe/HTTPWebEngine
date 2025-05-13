/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Functions.hpp                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: zouddach <zouddach@student.1337.ma>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/01/22 16:31:36 by zouddach          #+#    #+#             */
/*   Updated: 2025/04/17 15:21:51 by zouddach         ###   ########.fr       */
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
string                  getStatusMessage(int code);

#endif