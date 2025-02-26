/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Global.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: zouddach <zouddach@1337.student.ma>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/01/21 16:46:41 by zouddach          #+#    #+#             */
/*   Updated: 2025/02/15 22:10:05 by zouddach         ###   ########.fr       */
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
#include <sys/stat.h>
#include <sys/event.h>
#include <sys/time.h>
#include <sys/types.h>
#include <errno.h>
#include <sys/stat.h>
#include <dirent.h>
#include <fstream>

using namespace std;

// #include "functions.hpp"
// #include "Route.hpp"
// #include "Server.hpp"
// #include "File.hpp"
// #include "ConfigParser.hpp"
// #include "WebServer.hpp"

/* Color interface (IColor.hpp) and concrete classes (*Color.hpp) */
#include "Interfaces/Colors/IColor.hpp"
#include "Interfaces/Colors/RedColor.hpp"
#include "Interfaces/Colors/GreenColor.hpp"
#include "Interfaces/Colors/BlueColor.hpp"
#include "Interfaces/Colors/DefaultColor.hpp"
#include "Interfaces/Colors/BoldFont.hpp"

/* Declare color instances globally */
extern RedColor red;
extern GreenColor green;
extern BlueColor blue;
extern DefaultColor def;
extern BoldFont bold;



#endif