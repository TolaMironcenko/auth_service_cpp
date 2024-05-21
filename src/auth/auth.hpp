#ifndef AUTH_HPP
#define AUTH_HPP

#include <httplib.h>
#include <jwt-cpp/jwt.h>
#include <json.hpp>
#include <string>
#include <fstream>
#include "../includes.hpp"
#include <sstream>

void auth(const httplib::Request& request, httplib::Response& response);
bool verify_auth(const std::string token);

#endif // AUTH_HPP
