/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   functions.hpp                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mzeggaf <mzeggaf@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/01/22 16:31:36 by zouddach          #+#    #+#             */
/*   Updated: 2025/02/05 11:55:41 by mzeggaf          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef FUNCTIONS_HPP
#define FUNCTIONS_HPP

# include "Global.hpp"

string					trim(const string& str);
vector<string>			split(string str, char delimiter);
string					cpp11_replace(string  &str, string toReplace, string replacement);
pair<string, string>	parseKeyValue(const string& line);

#endif