#include "users.hpp"
#include "../colors.h"
#include "../includes.hpp"
#include "../auth/auth.hpp"
#include <jwt-cpp/jwt.h>
#include <syslog.h>

// functtion for change password for user
void change_password(const httplib::Request &request, httplib::Response &response) {
    std::cout << GREEN << request.path << RESET << "  " << request.method << std::endl;
    response.set_header("Access-Control-Allow-Origin", "*");
    if (request.body.empty()) {
        response.set_content(
            CHANGE_PASSWORD_REQUIRED_STRING,
            JSON_TYPE
        );
        syslog(LOG_ERR, "body is empty in change password request");
        return;
    }
    nlohmann::json json_body = nlohmann::json::parse(request.body);

    if (json_body["token"] == nullptr) {
        response.set_content(
            CHANGE_PASSWORD_REQUIRED_STRING,
            JSON_TYPE
        );
        syslog(LOG_ERR, "no token in change password request");
        return;
    }
    if ((json_body["userid"] == nullptr) && (json_body["old_password"] == nullptr)) {
        response.set_content(
            CHANGE_PASSWORD_REQUIRED_STRING,
            JSON_TYPE);
        syslog(LOG_ERR, "no old password is change password request");
        return;
    }
    if (json_body["new_password"] == nullptr) {
        response.set_content(CHANGE_PASSWORD_REQUIRED_STRING,
                             JSON_TYPE);
        syslog(LOG_ERR, "no new password in change password request");
        return;
    }
    if (!verify_auth(json_body["token"])) {
        response.set_content(STRING403, JSON_TYPE);
        syslog(LOG_ERR, "access reject in change password request");
        return;
    }

    std::string old_password;
    const std::string new_password = json_body["new_password"];

    if (json_body["userid"] == nullptr) { old_password = json_body["old_password"]; }

    jwt::decoded_jwt<jwt::traits::kazuho_picojson> decoded_token = jwt::decode(json_body["token"]);
    std::string userid = decoded_token.get_payload_json()["userId"].to_str();
    if (userid.empty()) {
        response.set_content(STRING403, JSON_TYPE);
        syslog(LOG_ERR, "access reject in change password request");
        return;
    }

    std::ifstream usersfile("users.json");
    nlohmann::json all_users = nlohmann::json::parse(usersfile);
    usersfile.close();

    std::string changepassuserid = userid;
    if (json_body["userid"] != nullptr) {
        changepassuserid = json_body["userid"];
        nlohmann::json response_user_data = nullptr;
        for (nlohmann::json &user: all_users) {
            if (user["id"] == userid) {
                response_user_data = user;
            }
        }
        if (response_user_data == nullptr) {
            response.set_content(STRING403, JSON_TYPE);
            syslog(LOG_ERR, "access reject in change password request");
            return;
        }
        if (response_user_data["is_superuser"] != "1") {
            response.set_content(STRING403, JSON_TYPE);
            syslog(LOG_ERR, "access reject in change password request");
            return;
        }
    }

    bool found = false;
    if (json_body["userid"] != nullptr) {
        for (nlohmann::basic_json<> &all_user: all_users) {
            if ((all_user["id"] == changepassuserid)) {
                all_user["password"] = new_password;
                found = true;
                std::stringstream syslogstring;
                syslogstring << "password changed for user ["  << all_user["username"] << "]";
                syslog(LOG_INFO, syslogstring.str().c_str());
                break;
            }
        }
    } else {
        for (nlohmann::basic_json<> &all_user: all_users) {
            if ((all_user["id"] == changepassuserid) && (all_user["password"] == old_password)) {
                all_user["password"] = new_password;
                found = true;
                std::stringstream syslogstring;
                syslogstring << "password changed for user ["  << all_user["username"] << "]";
                syslog(LOG_INFO, syslogstring.str().c_str());
                break;
            }
        }
    }
    if (!found) {
        response.set_content(R"({"status":"can't find this user"})", JSON_TYPE);
        syslog(LOG_ERR, "can't find this user in change password request");
        return;
    }
    std::ofstream usersfilew("users.json");
    usersfilew << all_users.dump(4);
    usersfilew.close();
    response.set_content(R"({"status":"ok"})", JSON_TYPE);
}
