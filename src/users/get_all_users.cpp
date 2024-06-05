#include "users.hpp"
#include "../colors.h"
#include "../includes.hpp"
#include "../auth/auth.hpp"
#include <jwt-cpp/jwt.h>

// function for get all users data without password
void get_all_users(const httplib::Request &request, httplib::Response &response) {
    std::cout << GREEN << request.path << RESET << "  " << request.method << std::endl;
    response.set_header("Access-Control-Allow-Origin", "*");
    if (request.body.empty()) {
        response.set_content(ACCESS_REQUIRED_STRING, JSON_TYPE);
        return;
    }
    nlohmann::json json_body = nlohmann::json::parse(request.body);

    if (json_body["token"] == nullptr) {
        response.set_content(ACCESS_REQUIRED_STRING, JSON_TYPE);
        return;
    }
    if (!verify_auth(json_body["token"])) {
        response.set_content(STRING403, JSON_TYPE);
        return;
    }
    jwt::decoded_jwt<jwt::traits::kazuho_picojson> decoded_token = jwt::decode(json_body["token"]);
    std::string userid;
    for (std::pair<const std::string, picojson::value> &e: decoded_token.get_payload_json()) {
        if (e.first == "userId") {
            userid = e.second.to_str();
            break;
        }
    }
    if (userid.empty()) {
        response.set_content(STRING403, JSON_TYPE);
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
        return;
    }
    if (response_user_data["is_superuser"] != "1") {
        response.set_content(STRING403, JSON_TYPE);
        return;
    }
    response.set_content(all_users.dump(), JSON_TYPE);
}
