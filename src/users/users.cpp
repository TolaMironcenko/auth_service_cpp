#include "users.hpp"

// function for get user data without password
void get_user(const httplib::Request& request, httplib::Response& response) {
    if (request.body == "") {response.set_content("{\"needed\":\"[token]\"}", "application/json");return;}
    nlohmann::json json_body = nlohmann::json::parse(request.body);

    if (json_body["token"] == nullptr) {response.set_content("{\"needed\":\"[token]\"}", "application/json");return;}
    if (!verify_auth(json_body["token"])) {response.set_content("{\"status\":\"403\"}", "application/json");return;}
    auto decoded_token = jwt::decode(json_body["token"]);
    std::string userid = "";
    for (auto& e : decoded_token.get_payload_json()) {
        if (e.first == "userId") {
            userid = e.second.to_str();
            break;
        }
    }
    if (userid == "") {response.set_content("{\"status\":\"403\"}", "application/json");return;}
    std::ifstream usersfile("users.json");
    nlohmann::json all_users = nlohmann::json::parse(usersfile);
    usersfile.close();
    nlohmann::json response_user_data = nullptr;
    for (auto& user : all_users) {
        if (user["id"] == userid) {
            response_user_data = user;
            break;
        }
    }
    if (response_user_data == nullptr) {response.set_content("{\"status\":\"403\"}", "application/json");return;}
    response_user_data.erase("password");
    response.set_content(response_user_data.dump(), "application/json");
}

// function for get all users data without password
void get_all_users(const httplib::Request& request, httplib::Response& response) {
    if (request.body == "") {response.set_content("{\"needed\":\"[token]\"}", "application/json");return;}
    nlohmann::json json_body = nlohmann::json::parse(request.body);

    if (json_body["token"] == nullptr) {response.set_content("{\"needed\":\"[token]\"}", "application/json");return;}
    if (!verify_auth(json_body["token"])) {response.set_content("{\"status\":\"403\"}", "application/json");return;}
    auto decoded_token = jwt::decode(json_body["token"]);
    std::string userid = "";
    for (auto& e : decoded_token.get_payload_json()) {
        if (e.first == "userId") {
            userid = e.second.to_str();
            break;
        }
    }
    if (userid == "") {response.set_content("{\"status\":\"403\"}", "application/json");return;}
    std::ifstream usersfile("users.json");
    nlohmann::json all_users = nlohmann::json::parse(usersfile);
    usersfile.close();
    nlohmann::json response_user_data = nullptr;
    for (auto& user : all_users) {
        if (user["id"] == userid) {
            response_user_data = user;
        }
        user.erase("password");
    }
    if (response_user_data == nullptr) {response.set_content("{\"status\":\"403\"}", "application/json");return;}
    if (response_user_data["is_superuser"] != "1") {response.set_content("{\"status\":\"403\"}", "application/json");return;}
    response.set_content(all_users.dump(), "application/json");
}

// function for access or reject authorization
void user_access(const httplib::Request& request, httplib::Response& response) {
    if (request.body == "") {response.set_content("{\"needed\":\"[token]\"}", "application/json");return;}
    nlohmann::json json_body = nlohmann::json::parse(request.body);

    if (json_body["token"] == nullptr) {response.set_content("{\"needed\":\"[token]\"}", "application/json");return;}
    if (!verify_auth(json_body["token"])) {response.set_content("{\"access\":\"reject\"}", "application/json");return;}
    response.set_content("{\"access\":\"success\"}", "application/json");
}
