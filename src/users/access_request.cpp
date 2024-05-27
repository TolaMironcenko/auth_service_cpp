#include "users.hpp"

// function for access or reject authorization
void user_access(const httplib::Request& request, httplib::Response& response) {
    response.set_header("Access-Control-Allow-Origin", "*");
    if (request.body == "") {response.set_content("{\"required\":\"[token]\"}", "application/json");return;}
    nlohmann::json json_body = nlohmann::json::parse(request.body);

    if (json_body["token"] == nullptr) {response.set_content("{\"required\":\"[token]\"}", "application/json");return;}
    if (!verify_auth(json_body["token"])) {response.set_content("{\"access\":\"reject\"}", "application/json");return;}
    response.set_content("{\"access\":\"success\"}", "application/json");
}
