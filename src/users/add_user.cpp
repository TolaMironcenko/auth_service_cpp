#include "users.hpp"
#include "../colors.h"
#include "../includes.hpp"
#include "../auth/auth.hpp"
#include <jwt.hpp>
#include <syslog.h>

#define ALPHABET "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789"

// function for generate random string
std::string get_random_string(const char *alphabet, int quantity) {
    srand(time(nullptr));
    std::string str;
    for (int i = 0; i < quantity; i++) {
        str += alphabet[0 + std::rand() % strlen(alphabet)];
    }
    return str;
}

// function for add user
void add_user(const httplib::Request &request, httplib::Response &response) {
    std::cout << GREEN << request.path << RESET << "  " << request.method << std::endl;
    response.set_header("Access-Control-Allow-Origin", "*");
    if (request.body.empty()) {
        response.set_content(ADD_USER_REQUIRED_STRING,
                             JSON_TYPE);
        syslog(LOG_ERR, "add user request body is empty");
        return;
    }
    nlohmann::json json_body = nlohmann::json::parse(request.body);

    if (json_body["token"] == nullptr) {
        response.set_content(ADD_USER_REQUIRED_STRING,
                             JSON_TYPE);
        syslog(LOG_ERR, "no token in add user request");
        return;
    }
    if (!verify_auth(json_body["token"])) {
        response.set_content(STRING403, JSON_TYPE);
        syslog(LOG_ERR, "access reject in add user request");
        return;
    }
    if (json_body["username"] == nullptr) {
        response.set_content(ADD_USER_REQUIRED_STRING,
                             JSON_TYPE);
        syslog(LOG_ERR, "no username in add user request");
        return;
    }
    if (json_body["password"] == nullptr) {
        response.set_content(ADD_USER_REQUIRED_STRING,
                             JSON_TYPE);
        syslog(LOG_ERR, "no password in add user request");
        return;
    }

    std::string userid = JWT::get_payload(json_body["token"], JWT_DEFAULT_SECRET)["userId"];
    if (userid.empty()) {
        response.set_content(STRING403, JSON_TYPE);
        syslog(LOG_ERR, "access reject in add user request");
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
    }
    if (response_user_data == nullptr) {
        response.set_content(STRING403, JSON_TYPE);
        syslog(LOG_ERR, "access reject in add user request");
        return;
    }
    if (response_user_data["is_superuser"] != "1") {
        response.set_content(STRING403, JSON_TYPE);
        syslog(LOG_ERR, "access reject in add user request");
        return;
    }

    const std::string username = json_body["username"];
    const std::string password = json_body["password"];

    for (nlohmann::json user: all_users) {
        if ((user["username"] == username)) {
            response.set_content(R"({"status":"user already exists"})", JSON_TYPE);
            syslog(LOG_ERR, "user already exists in add user request");
            return;
        }
    }

    std::stringstream new_user_data;

    new_user_data << R"({"id":")" << get_random_string(ALPHABET, 50) << R"(","username":")"
            << username << R"(","password":")" << password << R"(","group":)";
    (json_body["group"] != nullptr) ? new_user_data << json_body["group"] : new_user_data << "\"users\"";
    new_user_data << ",\"is_superuser\":";
    (json_body["is_superuser"] != nullptr) ? new_user_data << json_body["is_superuser"] : new_user_data << "\"0\"";
    new_user_data << "}";
    all_users.push_back(nlohmann::json::parse(new_user_data));

    std::ofstream usersfilew("users.json");
    usersfilew << all_users.dump(4);
    usersfilew.close();
    std::stringstream syslogstring;
    syslogstring << "user added ["  << username << "]";
    syslog(LOG_INFO, syslogstring.str().c_str());
    response.set_content(R"({"status":"ok"})", JSON_TYPE);
}
