/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   POSTResponse.hpp                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: zouddach <zouddach@student.1337.ma>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/17 15:03:42 by zouddach          #+#    #+#             */
/*   Updated: 2025/04/17 20:36:24 by zouddach         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef POSTRESPONSE_HPP
# define POSTRESPONSE_HPP

# include "Global.hpp"
#include "RequestProccessor.hpp"
#include "File.hpp"
#include "Server.hpp"

class POSTResponse {
    public:
    POSTResponse(RequestProccessor *Request);
    ~POSTResponse();
    POSTResponse(const POSTResponse &copy);
    POSTResponse &operator=(const POSTResponse &copy);
    
    RequestProccessor		*_Request;
    File					*_File;
    std::string				_Http_headers;

    void                    generateHttpHeaders(Server *server, int status_code);
    std::string             generateResponse();
};




#endif