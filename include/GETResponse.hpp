/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   GETResponse.hpp                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: zouddach <zouddach@student.1337.ma>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/10 00:34:43 by zouddach          #+#    #+#             */
/*   Updated: 2025/04/17 15:12:31 by zouddach         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef GETRESPONSE_HPP
#define GETRESPONSE_HPP

# include "Global.hpp"
#include "RequestProccessor.hpp"
#include "File.hpp"
#include "Server.hpp"
# include "Functions.hpp"

class GETResponse
{
    private:
    public:
        GETResponse(RequestProccessor *Request, File *File);
        GETResponse(const GETResponse &copy);
        GETResponse &operator=(const GETResponse &copy);
        ~GETResponse();
        RequestProccessor		*_Request;
		File					*_File;
		std::string				_Http_headers;
        void                    generateHttpHeaders(Server *server, int status_code);
        std::string             generateResponse();
};

#endif