#include "users.hpp"
#include "../colors.h"
#include "../includes.hpp"
#include "../auth/auth.hpp"
#include <jwt-cpp/jwt.h>

// function for change user data
void change_user(const httplib::Request &request, httplib::Response &response) {
    std::cout << GREEN << request.path << RESET << "  " << request.method << std::endl;
    response.set_header("Access-Control-Allow-Origin", "*");
    if (request.body.empty()) {
        response.set_content(
            CHANGE_USER_REQUIRED_STRING,
            JSON_TYPE);
        return;
    }
    nlohmann::json json_body = nlohmann::json::parse(request.body);

    if (json_body["token"] == nullptr) {
        response.set_content(
            CHANGE_USER_REQUIRED_STRING,
            JSON_TYPE
        );
        return;
    }
    if ((json_body["userid"] == nullptr) && (json_body["old_password"] == nullptr)) {
        response.set_content(
            CHANGE_USER_REQUIRED_STRING,
            JSON_TYPE);
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

    std::string changeusruserid = userid;
    if (json_body["userid"] != nullptr) {
        changeusruserid = json_body["userid"];
        nlohmann::json response_user_data = nullptr;
        for (nlohmann::json &user: all_users) {
            if (user["id"] == userid) {
                response_user_data = user;
            }
        }
        if (response_user_data == nullptr) {
            response.set_content(STRING403, JSON_TYPE);
            return;
        }
        if (response_user_data["is_superuser"] != "1") {
            response.set_content(STRING403, JSON_TYPE);
            return;
        }
    }

    bool found = false;
    if (json_body["userid"] != nullptr) {
        for (nlohmann::basic_json<> &all_user: all_users) {
            if ((all_user["id"] == changeusruserid)) {
                if (json_body["username"] != nullptr) { all_user["username"] = json_body["username"]; }
                if (json_body["password"] != nullptr) { all_user["password"] = json_body["password"]; }
                if (json_body["group"] != nullptr) { all_user["group"] = json_body["group"]; }
                if (json_body["is_superuser"] != nullptr) { all_user["is_superuser"] = json_body["is_superuser"]; }
                found = true;
                break;
            }
        }
    } else {
        for (nlohmann::basic_json<> &all_user: all_users) {
            if ((all_user["id"] == changeusruserid)) {
                if (json_body["username"] != nullptr) { all_user["username"] = json_body["username"]; }
                found = true;
                break;
            }
        }
    }
    if (!found) {
        response.set_content(R"({"status":"can't find this user"})", JSON_TYPE);
        return;
    }
    std::ofstream usersfilew("users.json");
    usersfilew << all_users.dump(4);
    usersfilew.close();
    response.set_content(R"({"status":"ok"})", JSON_TYPE);
}
