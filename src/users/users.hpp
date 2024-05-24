#ifndef USERS_HPP
#define USERS_HPP

#include <httplib.h>
#include <json.hpp>
#include "../auth/auth.hpp"

// function for get user data without password
void get_user(const httplib::Request& request, httplib::Response& response);
// function for get all users data without password
void get_all_users(const httplib::Request& request, httplib::Response& response);
// function for access or reject authorization
void user_access(const httplib::Request& request, httplib::Response& response);
// function for add user
void add_user(const httplib::Request& request, httplib::Response& response);
// function for delete user
void delete_user(const httplib::Request& request, httplib::Response& response);
// function for change password for user
void change_password(const httplib::Request& request, httplib::Response& response);

#endif // USERS_HPP
