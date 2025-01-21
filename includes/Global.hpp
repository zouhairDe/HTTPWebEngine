/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Global.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: zouddach <zouddach@1337.student.ma>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/01/21 16:46:41 by zouddach          #+#    #+#             */
/*   Updated: 2025/01/21 20:18:26 by zouddach         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef GLOBAL_HPP
#define GLOBAL_HPP

#include <algorithm>
#include <climits>
#include <iostream>
#include <limits>
#include <map>
#include <queue>
#include <string>
#include <vector>
#include <time.h>
#include <cmath>
#include <set>
#include <sstream>
#include <signal.h>
#include <fstream>
#include <istream>
#include <ostream>

using namespace std;

class File;
class Server;
class ConfigParser;
class Route;
class WebServer;

#include "ServerRoutes.hpp"
#include "Server.hpp"
#include "Parser.hpp"
#include "File.hpp"
#include "WebServer.hpp"

#endif