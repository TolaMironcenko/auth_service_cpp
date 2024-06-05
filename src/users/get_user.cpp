#include "users.hpp"
#include "../colors.h"
#include "../includes.hpp"
#include "../auth/auth.hpp"
#include <jwt-cpp/jwt.h>

// function for get user data without password
void get_user(const httplib::Request &request, httplib::Response &response) {
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
    std::string userid = decoded_token.get_payload_json()["userId"].to_str();
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
            break;
        }
    }
    if (response_user_data == nullptr) {
        response.set_content(STRING403, JSON_TYPE);
        return;
    }
    response_user_data.erase("password");
    response.set_content(response_user_data.dump(), JSON_TYPE);
}
