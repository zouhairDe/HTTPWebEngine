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
#include "Functions.hpp"
#include <string>
#include <cstring>

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