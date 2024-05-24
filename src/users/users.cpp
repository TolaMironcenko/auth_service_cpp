#include "users.hpp"
#include "sstream"

#define ALPHABET "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789"

std::string get_random_string(const char *alphabet, int quantity) {
    srand(time(NULL));
    std::string str;
    for (int i = 0; i < quantity; i++) {
        str += alphabet[0 + rand() % strlen(alphabet)];
    }
    return str;
}

// function for get user data without password
void get_user(const httplib::Request& request, httplib::Response& response) {
    if (request.body == "") {response.set_content("{\"required\":\"[token]\"}", "application/json");return;}
    nlohmann::json json_body = nlohmann::json::parse(request.body);

    if (json_body["token"] == nullptr) {response.set_content("{\"required\":\"[token]\"}", "application/json");return;}
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
    if (request.body == "") {response.set_content("{\"required\":\"[token]\"}", "application/json");return;}
    nlohmann::json json_body = nlohmann::json::parse(request.body);

    if (json_body["token"] == nullptr) {response.set_content("{\"required\":\"[token]\"}", "application/json");return;}
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
    usersfilew << all_users.dump();
    usersfilew.close();

    response.set_content("{\"status\":\"ok\"}", "application/json");
}

void del_user(const httplib::Request& request, httplib::Response& response) {
    if (request.body == "") {response.set_content("{\"required\":\"[token] or if you superuser [token,userid]\"}", "application/json");return;}
    nlohmann::json json_body = nlohmann::json::parse(request.body);

    if (json_body["token"] == nullptr) {response.set_content("{\"required\":\"[token] or if you superuser [token,userid]\"}", "application/json");return;}
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

    std::string deluserid = userid;
    if (json_body["userid"] != nullptr) {
        deluserid = json_body["userid"];
        nlohmann::json response_user_data = nullptr;
        for (auto& user : all_users) {
            if (user["id"] == userid) {
                response_user_data = user;
            }
        }
        if (response_user_data == nullptr) {response.set_content("{\"status\":\"403\"}", "application/json");return;}
        if (response_user_data["is_superuser"] != "1") {response.set_content("{\"status\":\"403\"}", "application/json");return;}
    }

    bool found = false;
    for (int i = 0; i < all_users.size(); i++) {
        if (all_users[i]["id"] == deluserid) {
            all_users.erase(i);
            found = true;
            break;
        }
    }
    if (!found) {response.set_content("{\"status\":\"can't find this user\"}", "application/json");return;}
    std::ofstream usersfilew("users.json");
    usersfilew << all_users.dump();
    usersfilew.close();
    response.set_content("{\"status\":\"ok\"}", "application/json");
}

// functtion for change password for user
void change_password(const httplib::Request& request, httplib::Response& response) {
    if (request.body == "") {response.set_content(
        "{\"required\":\"[token,old_password,new_password] or if you superuser [token,userid,new_password]\"}", 
        "application/json"
    );return;}
    nlohmann::json json_body = nlohmann::json::parse(request.body);

    if (json_body["token"] == nullptr) {response.set_content(
        "{\"required\":\"[token,old_password,new_password] or if you superuser [token,userid,new_password]\"}", 
        "application/json"
    );return;}
    if ((json_body["userid"] == nullptr) && (json_body["old_password"] == nullptr)) {
        response.set_content("{\"required\":\"[token,old_password,new_password] or if you superuser [token,userid,new_password]\"}", "application/json");
        return;
    }
    if (json_body["new_password"] == nullptr) {response.set_content("{\"required\":\"[token] or if you superuser [token,userid]\"}", "application/json");return;}
    if (!verify_auth(json_body["token"])) {response.set_content("{\"status\":\"403\"}", "application/json");return;}

    std::string old_password = "";
    const std::string new_password = json_body["new_password"];

    if (json_body["userid"] == nullptr) {old_password = json_body["old_password"];}

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

    std::string changepassuserid = userid;
    if (json_body["userid"] != nullptr) {
        changepassuserid = json_body["userid"];
        nlohmann::json response_user_data = nullptr;
        for (auto& user : all_users) {
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
            if ((all_users[i]["id"] == changepassuserid)) {
                all_users[i]["password"] = new_password;
                found = true;
                break;
            }
        }
    } else {
        for (int i = 0; i < all_users.size(); i++) {
            if ((all_users[i]["id"] == changepassuserid) && (all_users[i]["password"] == old_password)) {
                all_users[i]["password"] = new_password;
                found = true;
                break;
            }
        }
    }
    if (!found) {response.set_content("{\"status\":\"can't find this user\"}", "application/json");return;}
    std::ofstream usersfilew("users.json");
    usersfilew << all_users.dump();
    usersfilew.close();
    response.set_content("{\"status\":\"ok\"}", "application/json");
}

// function for access or reject authorization
void user_access(const httplib::Request& request, httplib::Response& response) {
    if (request.body == "") {response.set_content("{\"required\":\"[token]\"}", "application/json");return;}
    nlohmann::json json_body = nlohmann::json::parse(request.body);

    if (json_body["token"] == nullptr) {response.set_content("{\"required\":\"[token]\"}", "application/json");return;}
    if (!verify_auth(json_body["token"])) {response.set_content("{\"access\":\"reject\"}", "application/json");return;}
    response.set_content("{\"access\":\"success\"}", "application/json");
}
