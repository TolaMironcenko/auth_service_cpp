#ifndef AUTH_HPP
#define AUTH_HPP

#include <httplib.h>
#include <json.hpp>

// function for authorization users
void auth(const httplib::Request &request, httplib::Response &response);

// function for verify tokens
bool verify_auth(const std::string &token);

#endif // AUTH_HPP
