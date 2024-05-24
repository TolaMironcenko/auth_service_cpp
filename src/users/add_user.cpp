#include "users.hpp"

#define ALPHABET "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789"

// function for generate random string
std::string get_random_string(const char *alphabet, int quantity) {
    srand(time(NULL));
    std::string str;
    for (int i = 0; i < quantity; i++) {
        str += alphabet[0 + rand() % strlen(alphabet)];
    }
    return str;
}

// function for add user
void add_user(const httplib::Request& request, httplib::Response& response) {
    if (request.body == "") {response.set_content("{\"required\":\"[token,username,password]\",\"optional\":\"[group,is_superuser]\"}", "application/json");return;}
    nlohmann::json json_body = nlohmann::json::parse(request.body);

    if (json_body["token"] == nullptr) {response.set_content("{\"required\":\"[token,username,password]\",\"optional\":\"[group,is_superuser]\"}", "application/json");return;}
    if (!verify_auth(json_body["token"])) {response.set_content("{\"status\":\"403\"}", "application/json");return;}
    if (json_body["username"] == nullptr) {response.set_content("{\"required\":\"[token,username,password]\",\"optional\":\"[group,is_superuser]\"}", "application/json");return;}
    if (json_body["password"] == nullptr) {response.set_content("{\"required\":\"[token,username,password]\",\"optional\":\"[group,is_superuser]\"}", "application/json");return;}

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
    }
    if (response_user_data == nullptr) {response.set_content("{\"status\":\"403\"}", "application/json");return;}
    if (response_user_data["is_superuser"] != "1") {response.set_content("{\"status\":\"403\"}", "application/json");return;}

    const std::string username = json_body["username"];
    const std::string password = json_body["password"];

    for (auto& user : all_users) {
        if ((user["username"] == username)) {
            response.set_content("{\"status\":\"user already exists\"}", "application/json");return;
        }
    }

    std::stringstream new_user_data;

    new_user_data << "{\"id\":\"" << get_random_string(ALPHABET, 50) << "\",\"username\":\"" << username << "\",\"password\":\"" << password << "\",\"group\":";
    (json_body["group"] != nullptr) ? new_user_data << json_body["group"] : new_user_data << "\"users\"";
    new_user_data << ",\"is_superuser\":";
    (json_body["is_superuser"]  != nullptr) ? new_user_data << json_body["is_superuser"] : new_user_data << "\"0\"";
    new_user_data << "}";
    all_users.push_back(nlohmann::json::parse(new_user_data));

    std::ofstream usersfilew("users.json");
    usersfilew << all_users.dump(4);
    usersfilew.close();

    response.set_content("{\"status\":\"ok\"}", "application/json");
}
