/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ServerFunctions.cpp                                :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: zouddach <zouddach@student.1337.ma>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/02/13 18:06:27 by zouddach          #+#    #+#             */
/*   Updated: 2025/04/17 15:21:31 by zouddach         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "File.hpp"
# include "Global.hpp"
#include "Functions.hpp"
#include <string>
#include <cstring>

string generateHttpHeaders(const File& file, int status_code = 200) {
    // Get current time
    time_t now = time(0);
    struct tm* gmt = gmtime(&now);
    char date_buffer[128];
    strftime(date_buffer, sizeof(date_buffer), "%a, %d %b %Y %H:%M:%S GMT", gmt);

    // Get last modified time of file
    struct stat file_stat;
    char modified_buffer[128];
    if (stat(file.getPath().c_str(), &file_stat) == 0) {
        struct tm* modified_tm = gmtime(&file_stat.st_mtime);
        strftime(modified_buffer, sizeof(modified_buffer), "%a, %d %b %Y %H:%M:%S GMT", modified_tm);
    } else {
        strcpy(modified_buffer, date_buffer); // Fallback to current time
    }

    string http_headers = "HTTP/1.1 " + cpp11_toString(status_code) + " OK\r\n"
        "Date: " + string(date_buffer) + "\r\n"
        "Server: HTTPWebEngine v0.6.9\r\n"
        "Last-Modified: " + string(modified_buffer) + "\r\n"
        "Content-Length: " + cpp11_toString(file.getSize()) + "\r\n"
        "Content-Type: text/html; charset=UTF-8\r\n"//todo later: file.getContentType()
        "Connection: keep-alive\r\n\r\n";

    return http_headers;
}

string  getStatusMessage(int code)
{
    switch (code)
    {
        case 200:
            return " OK\r\n";
        case 400:
            return " Bad Request\r\n";
        case 403:
            return " Forbidden\r\n";
        case 404:
            return " Not Found\r\n";
        case 500:
            return " Internal Server Error\r\n";
        case 501:
            return " Not Implemented\r\n";
        case 502:
            return " Bad Gateway\r\n";
        case 503:
            return " Service Unavailable\r\n";  
        case 504:
            return " Gateway Timeout\r\n";
        default:
            return " Unknown Status\r\n";
    }
}

string generateSessionID() {
    size_t length = 32;
    static const char charset[] =
        "0123456789"
        "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
        "abcdefghijklmnopqrstuvwxyz";
    static const size_t charsetSize = sizeof(charset) - 1;

    string result;
    for (size_t i = 0; i < length; ++i) {
        result += charset[rand() % charsetSize];
    }
    return result;
}

string getCurrentTime() {
    std::ostringstream oss;
    oss << time(NULL); // seconds since epoch
    return oss.str();
}
