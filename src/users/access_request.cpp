#include "users.hpp"
#include "../colors.h"
#include "../includes.hpp"
#include "../auth/auth.hpp"

// function for access or reject authorization
void user_access(const httplib::Request &request, httplib::Response &response) {
    std::cout << GREEN << request.path << RESET << "  " << request.method << std::endl;
    response.set_header("Access-Control-Allow-Origin", "*");
    if (request.body.empty()) {
        response.set_content(ACCESS_REQUIRED_STRING, JSON_TYPE);
        return;
    }
    nlohmann::json json_body = nlohmann::json::parse(request.body);

    if (json_body["token"] == nullptr) {
        response.set_content(ACCESS_REQUIRED_STRING, JSON_TYPE);
        return;
    }
    if (!verify_auth(json_body["token"])) {
        response.set_content(R"({"access":"reject"})", JSON_TYPE);
        return;
    }
    response.set_content(R"({"access":"success"})", JSON_TYPE);
}
