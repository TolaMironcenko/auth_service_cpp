#include <users/users.hpp>
#include <colors.h>
#include <includes.hpp>
#include <auth/auth.hpp>
#include <jwt.hpp>
#include <syslog.h>

// function for get user data without password
void get_user(const httplib::Request &request, httplib::Response &response) {
    std::cout << GREEN << request.path << RESET << "  " << request.method << std::endl;
    response.set_header("Access-Control-Allow-Origin", "*");
    if (request.body.empty()) {
        response.set_content(ACCESS_REQUIRED_STRING, JSON_TYPE);
        syslog(LOG_ERR, "get user request body is empty");
        return;
    }
    nlohmann::json json_body = nlohmann::json::parse(request.body);

    if (json_body["token"] == nullptr) {
        response.set_content(ACCESS_REQUIRED_STRING, JSON_TYPE);
        syslog(LOG_ERR, "no token in get user request");
        return;
    }
    if (!verify_auth(json_body["token"])) {
        response.set_content(STRING403, JSON_TYPE);
        syslog(LOG_ERR, "access reject in get user request");
        return;
    }
    std::string userid = JWT::get_payload(json_body["token"], JWT_SECRET_KEY)["userId"];
    if (userid.empty()) {
        response.set_content(STRING403, JSON_TYPE);
        syslog(LOG_ERR, "access reject in get user request");
        return;
    }
    std::ifstream usersfile("users.json");
    nlohmann::json all_users = nlohmann::json::parse(usersfile);
    usersfile.close();
    nlohmann::json response_user_data = nullptr;
    for (nlohmann::json &user: all_users) {
        if (user["id"] == userid) {
            response_user_data = user;
            break;
        }
    }
    if (response_user_data == nullptr) {
        response.set_content(STRING403, JSON_TYPE);
        syslog(LOG_ERR, "access reject in get user request");
        return;
    }
    response_user_data.erase("password");
    response.set_content(response_user_data.dump(), JSON_TYPE);
}
