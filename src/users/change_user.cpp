#include <users/users.hpp>
#include <colors.h>
#include <includes.hpp>
#include <auth/auth.hpp>
#include <jwt.hpp>
#include <syslog.h>

// function for change user data
void change_user(const httplib::Request &request, httplib::Response &response) {
    std::cout << GREEN << request.path << RESET << "  " << request.method << std::endl;
    response.set_header("Access-Control-Allow-Origin", "*");
    if (request.body.empty()) {
        response.set_content(
            CHANGE_USER_REQUIRED_STRING,
            JSON_TYPE);
        syslog(LOG_ERR, "change user request body is empty");
        return;
    }
    nlohmann::json json_body = nlohmann::json::parse(request.body);

    if (json_body["token"] == nullptr) {
        response.set_content(
            CHANGE_USER_REQUIRED_STRING,
            JSON_TYPE
        );
        syslog(LOG_ERR, "no token in change user request");
        return;
    }
    if ((json_body["userid"] == nullptr) && (json_body["old_password"] == nullptr)) {
        response.set_content(
            CHANGE_USER_REQUIRED_STRING,
            JSON_TYPE);
        syslog(LOG_ERR, "no old_password in change user request");
        return;
    }
    if (!verify_auth(json_body["token"])) {
        response.set_content(STRING403, JSON_TYPE);
        syslog(LOG_ERR, "access reject in change user request");
        return;
    }

    std::string userid = JWT::get_payload(json_body["token"], JWT_SECRET_KEY)["userId"];
    if (userid.empty()) {
        response.set_content(STRING403, JSON_TYPE);
        syslog(LOG_ERR, "access reject in change user request");
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
            syslog(LOG_ERR, "access reject in change user request");
            return;
        }
        if (response_user_data["is_superuser"] != "1") {
            response.set_content(STRING403, JSON_TYPE);
            syslog(LOG_ERR, "access reject in change user request");
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
                std::stringstream syslogstring;
                syslogstring << "userdata changed for user ["  << all_user["username"] << "]";
                syslog(LOG_INFO, syslogstring.str().c_str());
                break;
            }
        }
    } else {
        for (nlohmann::basic_json<> &all_user: all_users) {
            if ((all_user["id"] == changeusruserid)) {
                if (json_body["username"] != nullptr) { all_user["username"] = json_body["username"]; }
                found = true;
                std::stringstream syslogstring;
                syslogstring << "userdata changed for user ["  << all_user["username"] << "]";
                syslog(LOG_INFO, syslogstring.str().c_str());
                break;
            }
        }
    }
    if (!found) {
        response.set_content(R"({"status":"can't find this user"})", JSON_TYPE);
        syslog(LOG_ERR, "can't find this user in change user request");
        return;
    }
    std::ofstream usersfilew("users.json");
    usersfilew << all_users.dump(4);
    usersfilew.close();
    response.set_content(R"({"status":"ok"})", JSON_TYPE);
}
