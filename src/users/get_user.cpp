#include "users.hpp"

// function for get user data without password
void get_user(const httplib::Request& request, httplib::Response& response) {
    if (request.body == "") {response.set_content("{\"required\":\"[token]\"}", "application/json");return;}
    nlohmann::json json_body = nlohmann::json::parse(request.body);

    if (json_body["token"] == nullptr) {response.set_content("{\"required\":\"[token]\"}", "application/json");return;}
    if (!verify_auth(json_body["token"])) {response.set_content("{\"status\":\"403\"}", "application/json");return;}
    jwt::decoded_jwt<jwt::traits::kazuho_picojson> decoded_token = jwt::decode(json_body["token"]);
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
    for (nlohmann::json& user : all_users) {
        if (user["id"] == userid) {
            response_user_data = user;
            break;
        }
    }
    if (response_user_data == nullptr) {response.set_content("{\"status\":\"403\"}", "application/json");return;}
    response_user_data.erase("password");
    response.set_content(response_user_data.dump(), "application/json");
}
