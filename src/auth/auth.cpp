#include "auth.hpp"
#include "../colors.h"
#include "../includes.hpp"
#include <jwt.hpp>
#include <syslog.h>

// function for authorization users
void auth(const httplib::Request &request, httplib::Response &response) {
    std::cout << GREEN << request.path << RESET << "  " << request.method << std::endl;
    response.set_header("Access-Control-Allow-Origin", "*");
    if (request.body.empty()) {
        response.set_content(AUTH_REQUIRED_STRING, JSON_TYPE);
        syslog(LOG_ERR, "no body in auth request");
        return;
    }
    nlohmann::json json_body = nlohmann::json::parse(request.body);

    if (json_body["username"] == nullptr) {
        response.set_content(AUTH_REQUIRED_STRING, JSON_TYPE);
        syslog(LOG_ERR, "no username in auth request");
        return;
    }
    if (json_body["password"] == nullptr) {
        response.set_content(AUTH_REQUIRED_STRING, JSON_TYPE);
        syslog(LOG_ERR, "no password in auth request");
        return;
    }
    const std::string request_username = json_body["username"];
    const std::string request_password = json_body["password"];

    std::ifstream usersfile("users.json");
    nlohmann::json all_users = nlohmann::json::parse(usersfile);
    usersfile.close();

    std::string userid;
    for (nlohmann::basic_json<> user: all_users) {
        if ((user["username"] == request_username) && (user["password"] == request_password)) {
            userid = user["id"];
            break;
        }
    }
    if (userid.empty()) {
        response.set_content(STRING403, JSON_TYPE);
        syslog(LOG_ERR, "auth request rejected");
        return;
    }
    
    nlohmann::json headers = {
        {"alg", "HS256"},
        {"typ", "JWT"}
    };

    nlohmann::json payload {
        {"userId", userid}
    };

    std::string token = JWT::createJWT(headers.dump(), payload.dump(), JWT_DEFAULT_SECRET);

    std::stringstream response_json;
    response_json << R"({"token":")" << token.c_str() << "\"}";
    syslog(LOG_INFO, "auth request success");
    response.set_content(response_json.str(), JSON_TYPE);
}
