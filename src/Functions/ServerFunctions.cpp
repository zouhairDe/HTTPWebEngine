/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ServerFunctions.cpp                                :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: zouddach <zouddach@1337.student.ma>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/02/13 18:06:27 by zouddach          #+#    #+#             */
/*   Updated: 2025/02/13 18:14:23 by zouddach         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "File.hpp"
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