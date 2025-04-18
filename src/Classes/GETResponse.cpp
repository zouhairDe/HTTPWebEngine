/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   GETResponse.cpp                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: zouddach <zouddach@student.1337.ma>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/10 01:23:27 by zouddach          #+#    #+#             */
/*   Updated: 2025/04/17 15:35:37 by zouddach         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

# include "GETResponse.hpp"

GETResponse::GETResponse(RequestProccessor *Request, File *File)
{
    _Request = Request;
    _File = File;
}

GETResponse::~GETResponse()
{
}

GETResponse::GETResponse(const GETResponse &copy)
{
    if (this != &copy)
    {
        _Request = copy._Request;
        // _File = new File(*copy._File);//no need hhhhh may3i9och
        _File = copy._File;
        _Http_headers = copy._Http_headers;
    }
}

GETResponse &GETResponse::operator=(const GETResponse &copy)
{
    if (this != &copy)
    {
        _Request = copy._Request;
        _File = copy._File;
        _Http_headers = copy._Http_headers;
    }
    return *this;
}

void    GETResponse::generateHttpHeaders(int status_code)
{
    _Http_headers = "HTTP/1.1 " + cpp11_toString(status_code) + getStatusMessage(status_code);
    _Http_headers += "Server: webserv/1.0.0 (Ubuntu)\r\n";
    _Http_headers += "Content-Type: text/html\r\n";
    _Http_headers += "Content-Length: " + cpp11_toString(_File->getSize()) + "\r\n";
    _Http_headers += "Connection: close\r\n";//to change ater , from request
    _Http_headers += "\r\n";
}

std::string     ReturnServerErrorPage(Server *server, int status_code)//should be a global function
{
    /* File    error_page(server->getErrorPage());// till we have the server */(void)server;
    File    error_page("/home/zouddach/Desktop/webserver/error/404.html");
    std::string response = "HTTP/1.1" + cpp11_toString(status_code) + " " + getStatusMessage(status_code);
    response += "Server: webserv/1.0.0 (Ubuntu)\r\n";
    response += "Content-Type: text/html\r\n";
    response += "Content-Length: " + cpp11_toString(error_page.getSize()) + "\r\n";
    response += "Connection: close\r\n";
    response += "\r\n";
    response += error_page.getData();
    return response;
}

std::string    GETResponse::generateResponse()
{
    std::string method = _Request->getMethod();//this will go in the caller of this function , db just 4 tests
    if (_File->exists() == false)//had File atkon fih uri dial request
        return ReturnServerErrorPage(_Request->_server, 404);
    //Handling this func's method  "GET"

    else// if no cgi
    {
        generateHttpHeaders(200);
        std::string response = _Http_headers;
        cout << green << "hmmmmmmmmmmmmmm File ppath is: "  << _File->getPath() << def << endl;
        response += string(_File->getData());
        return response;
    }
    //maybe mn b3d ykhsn nhandliw l Body dia GET for cgi..
}

