
#ifndef CLIENT_HPP
#define CLIENT_HPP

#pragma once

#include <string>
#include <vector>

class Server;

class Client {
    public:
        Client();
        ~Client();

        Server      *_Parent;
        
        int         socket;
        bool        done;
        std::string request;
};

#endif
