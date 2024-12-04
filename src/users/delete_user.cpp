#include <users/users.hpp>
#include <colors.h>
#include <includes.hpp>
#include <auth/auth.hpp>
#include <jwt.hpp>
#include <syslog.h>

// function for delete user
void delete_user(const httplib::Request &request, httplib::Response &response) {
    std::cout << GREEN << request.path << RESET << "  " << request.method << std::endl;
    response.set_header("Access-Control-Allow-Origin", "*");
    if (request.body.empty()) {
        response.set_content(
            DELETE_USER_REQUIRED_STRING,
            JSON_TYPE);
        syslog(LOG_ERR, "delete user request body is empty");
        return;
    }
    nlohmann::json json_body = nlohmann::json::parse(request.body);

    if (json_body["token"] == nullptr) {
        response.set_content(
            DELETE_USER_REQUIRED_STRING,
            JSON_TYPE);
        syslog(LOG_ERR, "no token in delete user request");
        return;
    }
    if (!verify_auth(json_body["token"])) {
        response.set_content(STRING403, JSON_TYPE);
        syslog(LOG_ERR, "access reject in delete user request");
        return;
    }

    std::string userid = JWT::get_payload(json_body["token"], JWT_SECRET_KEY)["userId"];
    if (userid.empty()) {
        response.set_content(STRING403, JSON_TYPE);
        syslog(LOG_ERR, "access reject in delete user request");
        return;
    }

    std::ifstream usersfile("users.json");
    nlohmann::json all_users = nlohmann::json::parse(usersfile);
    usersfile.close();

    std::string deluserid = userid;
    if (json_body["userid"] != nullptr) {
        deluserid = json_body["userid"];
        nlohmann::json response_user_data = nullptr;
        for (nlohmann::json &user: all_users) {
            if (user["id"] == userid) {
                response_user_data = user;
            }
        }
        if (response_user_data == nullptr) {
            response.set_content(STRING403, JSON_TYPE);
            syslog(LOG_ERR, "access reject in delete user request");
            return;
        }
        if (response_user_data["is_superuser"] != "1") {
            response.set_content(STRING403, JSON_TYPE);
            syslog(LOG_ERR, "access reject in delete user request");
            return;
        }
    }

    bool found = false;
    for (size_t i = 0; i < all_users.size(); i++) {
        if (all_users[i]["id"] == deluserid) {
            std::stringstream syslogstring;
            syslogstring << "user deleted ["  << all_users[i]["username"] << "]";
            all_users.erase(i);
            found = true;
            syslog(LOG_INFO, syslogstring.str().c_str());
            break;
        }
    }
    if (!found) {
        response.set_content(R"({"status":"can't find this user"})", JSON_TYPE);
        syslog(LOG_ERR, "can't find this user in delete user request");
        return;
    }
    std::ofstream usersfilew("users.json");
    usersfilew << all_users.dump(4);
    usersfilew.close();
    response.set_content(R"({"status":"ok"})", JSON_TYPE);
}
