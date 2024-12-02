#include <users/users.hpp>
#include <colors.h>
#include <includes.hpp>
#include <auth/auth.hpp>
#include <syslog.h>
#include <json.hpp>

// function for access or reject authorization
void user_access(const httplib::Request &request, httplib::Response &response) {
    std::cout << GREEN << request.path << RESET << "  " << request.method << std::endl;
    response.set_header("Access-Control-Allow-Origin", "*");
    if (request.body.empty()) {
        response.set_content(ACCESS_REQUIRED_STRING, JSON_TYPE);
        syslog(LOG_ERR, "access request body is emty");
        return;
    }
    nlohmann::json json_body = nlohmann::json::parse(request.body);

    if (json_body["token"] == nullptr) {
        response.set_content(ACCESS_REQUIRED_STRING, JSON_TYPE);
        syslog(LOG_ERR, "no token in acces request");
        return;
    }
    if (!verify_auth(json_body["token"])) {
        response.set_content(R"({"access":"reject"})", JSON_TYPE);
        syslog(LOG_ERR, "access rejected");
        return;
    }
    response.set_content(R"({"access":"success"})", JSON_TYPE);
    syslog(LOG_INFO, "access success");
}
