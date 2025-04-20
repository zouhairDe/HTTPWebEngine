/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   POSTResponse.cpp                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: zouddach <zouddach@student.1337.ma>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/17 15:06:15 by zouddach          #+#    #+#             */
/*   Updated: 2025/04/17 20:55:04 by zouddach         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

# include "POSTResponse.hpp"

POSTResponse::POSTResponse(RequestProccessor *Request)
{
    _Request = Request;
}

POSTResponse::~POSTResponse()
{
}

POSTResponse::POSTResponse(const POSTResponse &copy)
{
    if (this != &copy)
    {
        _Request = copy._Request;
        _File = copy._File;
        _Http_headers = copy._Http_headers;
    }
}

POSTResponse &POSTResponse::operator=(const POSTResponse &copy)
{
    if (this != &copy)
    {
        _Request = copy._Request;
        _File = copy._File;
        _Http_headers = copy._Http_headers;
    }
    return *this;
}


void POSTResponse::generateHttpHeaders(Server *server, int status_code)
{
    (void)server;
    _Http_headers = "HTTP/1.1 " + cpp11_toString(status_code) + " " + getStatusMessage(status_code);
    _Http_headers += "Server: webserv/1.0.0 (Ubuntu)\r\n";
    _Http_headers += "Content-Type: text/html\r\n";
    _Http_headers += "Connection: close\r\n";//to change ater , from request
    _Http_headers += "Content-Length: 0\r\n";
    _Http_headers += "\r\n";
}

string   POSTResponse::generateResponse()
{
    //since mazal makankhdch upload_store andiro ./body/ default (ta ngnix kismig body)
    string store_path = "./body/";
    Route *route = _Request->_server->getRouteFromUri("\"" + _Request->getUri() + "\"");
    // cout << bold << red << "Server : " << *_Request->_server << def << endl;
    if (route && !route->getUploadStore().empty())
        store_path = route->getUploadStore();
    
    //create the directory if it does not exist
    if (mkdir(store_path.c_str(), 0777) == -1)
    {
        if (errno != EEXIST)
        {
            std::cerr << "Error creating directory: " << std::endl;
            return "";
        }
    }

    // we look for the filneme after Content-Disposition
    // and we create the file in the upload store 
    std::string filename = _Request->getStoreFileName();
    std::string file_path = store_path ;
    // cout << bold << red << "store path: " << file_path << def << endl;

    generateHttpHeaders(_Request->_server, 200);
    
    return (_Http_headers);
    
}