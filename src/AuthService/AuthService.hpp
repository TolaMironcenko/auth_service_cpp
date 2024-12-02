#pragma once

#include <iostream>
#include <httplib.h>

#define CORS_ENABLE

class AuthService 
{
private:
    httplib::Server srv;
public:
    AuthService();
    ~AuthService();
    int serve(const std::string &ip, int port, int socket_flags = 0);
};
