#include "users.hpp"
#include "../colors.h"
#include "../includes.hpp"
#include "../auth/auth.hpp"
#include <jwt.hpp>
#include <syslog.h>

// function for get all users data without password
void get_all_users(const httplib::Request &request, httplib::Response &response) {
    std::cout << GREEN << request.path << RESET << "  " << request.method << std::endl;
    response.set_header("Access-Control-Allow-Origin", "*");
    if (request.body.empty()) {
        response.set_content(ACCESS_REQUIRED_STRING, JSON_TYPE);
        syslog(LOG_ERR, "get all users request body is empty");
        return;
    }
    nlohmann::json json_body = nlohmann::json::parse(request.body);

    if (json_body["token"] == nullptr) {
        response.set_content(ACCESS_REQUIRED_STRING, JSON_TYPE);
        syslog(LOG_ERR, "no token in get all users");
        return;
    }
    if (!verify_auth(json_body["token"])) {
        response.set_content(STRING403, JSON_TYPE);
        syslog(LOG_ERR, "access reject in get all user request");
        return;
    }
    std::string userid = JWT::get_payload(json_body["token"], JWT_DEFAULT_SECRET)["userId"];
    if (userid.empty()) {
        response.set_content(STRING403, JSON_TYPE);
        syslog(LOG_ERR, "access reject in get all user request");
        return;
    }
    std::ifstream usersfile("users.json");
    nlohmann::json all_users = nlohmann::json::parse(usersfile);
    usersfile.close();
    nlohmann::json response_user_data = nullptr;
    for (nlohmann::json &user: all_users) {
        if (user["id"] == userid) {
            response_user_data = user;
        }
        user.erase("password");
    }
    if (response_user_data == nullptr) {
        response.set_content(STRING403, JSON_TYPE);
        syslog(LOG_ERR, "access reject in get all user request");
        return;
    }
    if (response_user_data["is_superuser"] != "1") {
        response.set_content(STRING403, JSON_TYPE);
        syslog(LOG_ERR, "access reject in get all user request");
        return;
    }
    response.set_content(all_users.dump(), JSON_TYPE);
}
