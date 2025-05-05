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
#include <sys/time.h>
#include <sys/types.h>
#include <errno.h>
#include <sys/stat.h>
#include <dirent.h>
#include <fstream>
#include <stdlib.h>
#include <sys/wait.h>
# include <fcntl.h>


using namespace std;

#define nullptr NULL
#define FORBIDDEN_STATUS_CODE 403
#define NOT_FOUND_STATUS_CODE 404
#define OK_STATUS_CODE 200
#define BAD_REQUEST_STATUS_CODE 400
#define INTERNAL_SERVER_ERROR_STATUS_CODE 500
#define MOVED_PERMANENTLY_STATUS_CODE 301
#define FOUND_STATUS_CODE 302
#define NOT_ALLOWED_STATUS_CODE 405
#define REQUEST_ENTITY_TOO_LARGE_STATUS_CODE 413
#define REQUEST_LENGTH_REQUIRED_STATUS_CODE 411

const int MAX_CLIENTS = 1000;
const int CGI_TIMEOUT = 5;//to implement later
const int MAX_CGI_CALLS = 50;

string generateSessionID();
string getCurrentTime();

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