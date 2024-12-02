#pragma once

#include <httplib.h>

// function for authorization users
void auth(const httplib::Request &request, httplib::Response &response);

// function for verify tokens
bool verify_auth(const std::string &token);
