/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Global.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mzeggaf <mzeggaf@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/01/21 16:46:41 by zouddach          #+#    #+#             */
/*   Updated: 2025/02/05 11:56:22 by mzeggaf          ###   ########.fr       */
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
#include <netinet/in.h>
#include <sys/socket.h>
#include <stdexcept>
#include <unistd.h>

using namespace std;

class File;
class Server;
class ConfigParser;
class Route;
class WebServer;

#include "functions.hpp"
#include "cpp11.hpp"
#include "ServerRoutes.hpp"
#include "Server.hpp"
#include "File.hpp"
#include "Parser.hpp"
#include "WebServer.hpp"

/* Color interface (IColor.hpp) and concrete classes (*Color.hpp) */
#include "interfaces/Colors/IColor.hpp"
#include "interfaces/Colors/RedColor.hpp"
#include "interfaces/Colors/GreenColor.hpp"
#include "interfaces/Colors/BlueColor.hpp"
#include "interfaces/Colors/DefaultColor.hpp"
#include "interfaces/Colors/BoldFont.hpp"

/* Declare color instances globally */
extern RedColor red;
extern GreenColor green;
extern BlueColor blue;
extern DefaultColor def;
extern BoldFont bold;

#endif