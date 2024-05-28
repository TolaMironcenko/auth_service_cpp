#include "users.hpp"

void change_user(const httplib::Request& request, httplib::Response& response) {
    response.set_header("Access-Control-Allow-Origin", "*");
    if (request.body == "") {response.set_content(
        "{\"required\":\"[token] or if you superuser [token,userid]\",\"optional\":\"[username,password,is_superuser,group]\"}", 
        "application/json"
    );return;}
    nlohmann::json json_body = nlohmann::json::parse(request.body);

    if (json_body["token"] == nullptr) {response.set_content(
        "{\"required\":\"[token] or if you superuser [token,userid]\",\"optional\":\"[username,password,is_superuser,group]\"}", 
        "application/json"
    );return;}
    if ((json_body["userid"] == nullptr) && (json_body["old_password"] == nullptr)) {
        response.set_content("{\"required\":\"[token] or if you superuser [token,userid]\",\"optional\":\"[username,password,is_superuser,group]\"}", "application/json");
        return;
    }
    // if (json_body["new_password"] == nullptr) {response.set_content("{\"required\":\"[token] or if you superuser [token,userid]\"}", "application/json");return;}
    if (!verify_auth(json_body["token"])) {response.set_content("{\"status\":\"403\"}", "application/json");return;}

    // std::string old_password = "";
    // const std::string new_password = json_body["new_password"];

    // if (json_body["userid"] == nullptr) {old_password = json_body["old_password"];}

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

    std::string changeusruserid = userid;
    if (json_body["userid"] != nullptr) {
        changeusruserid = json_body["userid"];
        nlohmann::json response_user_data = nullptr;
        for (nlohmann::json& user : all_users) {
            if (user["id"] == userid) {
                response_user_data = user;
            }
        }
        if (response_user_data == nullptr) {response.set_content("{\"status\":\"403\"}", "application/json");return;}
        if (response_user_data["is_superuser"] != "1") {response.set_content("{\"status\":\"403\"}", "application/json");return;}
    }

    bool found = false;
    if (json_body["userid"] != nullptr) {
        for (int i = 0; i < all_users.size(); i++) {
            if ((all_users[i]["id"] == changeusruserid)) {
                if (json_body["username"] != nullptr) {all_users[i]["username"] = json_body["username"];}
                if (json_body["password"] != nullptr) {all_users[i]["password"] = json_body["password"];}
                if (json_body["group"] != nullptr) {all_users[i]["group"] = json_body["group"];}
                if (json_body["is_superuser"] != nullptr) {all_users[i]["is_superuser"] = json_body["is_superuser"];}
                found = true;
                break;
            }
        }
    } else {
        for (int i = 0; i < all_users.size(); i++) {
            if ((all_users[i]["id"] == changeusruserid)) {
                if (json_body["username"] != nullptr) {all_users[i]["username"] = json_body["username"];}
                found = true;
                break;
            }
        }
    }
    if (!found) {response.set_content("{\"status\":\"can't find this user\"}", "application/json");return;}
    std::ofstream usersfilew("users.json");
    usersfilew << all_users.dump(4);
    usersfilew.close();
    response.set_content("{\"status\":\"ok\"}", "application/json");
}
