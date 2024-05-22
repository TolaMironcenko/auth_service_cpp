#include "auth.hpp"

// function for authorization users
void auth(const httplib::Request& request, httplib::Response& response) {
    if (request.body == "") {response.set_content("{\"needed\":\"[username,password]\"}", "application/json");return;}
    nlohmann::json json_body = nlohmann::json::parse(request.body);

    if (json_body["username"] == nullptr) {response.set_content("{\"needed\":\"[username,password]\"}", "application/json");return;}
    if (json_body["password"] == nullptr) {response.set_content("{\"needed\":\"[username,password]\"}", "application/json");return;}
    const std::string request_username = json_body["username"];
    const std::string request_password = json_body["password"];

    std::ifstream usersfile("users.json");
    nlohmann::json all_users = nlohmann::json::parse(usersfile);
    usersfile.close();

    std::string userid = "";
    for (int i = 0; i < all_users.size(); i++) {
        if ((all_users[i]["username"] == request_username) && (all_users[i]["password"] == request_password)) {
            userid = all_users[i]["id"];
            break;
        }
    }

    if (userid == "") {response.set_content("{\"status\":\"403\"}", "application/json");return;}

    auto token = jwt::create()
        .set_type("JWT")
        .set_issuer("auth0")
        .set_payload_claim("userId", jwt::claim(userid))
        .sign(jwt::algorithm::hs256{JWT_SECRET_KEY});

    std::stringstream response_json;
    response_json << "{\"token\":\"" << token << "\"}";
    response.set_content(response_json.str(), "application/json");
}

// function for verify tokens
bool verify_auth(const std::string token) {
    try {
        // parse token
        const auto decoded = jwt::decode(token);

        // validate token
        const auto verifier = jwt::verify()
                                .allow_algorithm(jwt::algorithm::hs256{JWT_SECRET_KEY})
                                .with_issuer("auth0");

        verifier.verify(decoded);
        return true;
    }
    catch (const std::system_error& e) {
        std::cout << "Verification error: " << e.what() << std::endl;
        return false;
    }
    catch (...) {
        return false;
    }
}
