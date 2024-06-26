#include "users.hpp"
#include "../colors.h"
#include "../includes.hpp"
#include "../auth/auth.hpp"
#include <jwt-cpp/jwt.h>

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
        return;
    }
    nlohmann::json json_body = nlohmann::json::parse(request.body);

    if (json_body["token"] == nullptr) {
        response.set_content(ADD_USER_REQUIRED_STRING,
                             JSON_TYPE);
        return;
    }
    if (!verify_auth(json_body["token"])) {
        response.set_content(STRING403, JSON_TYPE);
        return;
    }
    if (json_body["username"] == nullptr) {
        response.set_content(ADD_USER_REQUIRED_STRING,
                             JSON_TYPE);
        return;
    }
    if (json_body["password"] == nullptr) {
        response.set_content(ADD_USER_REQUIRED_STRING,
                             JSON_TYPE);
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

    const std::string username = json_body["username"];
    const std::string password = json_body["password"];

    for (nlohmann::json user: all_users) {
        if ((user["username"] == username)) {
            response.set_content(R"({"status":"user already exists"})", JSON_TYPE);
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

    response.set_content(R"({"status":"ok"})", JSON_TYPE);
}
