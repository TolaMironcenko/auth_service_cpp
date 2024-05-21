#include <iostream>
#include <fstream>
#include <httplib.h>
#include <json.hpp>
#include <jwt-cpp/jwt.h>

#include "auth/auth.hpp"

int main() {
    httplib::Server srv;

    srv.Post("/api/token", auth);

    srv.Get("/api/users", [](const httplib::Request& request, httplib::Response& response) {
        std::ifstream usersfile("users.json");
        nlohmann::json usersdata = nlohmann::json::parse(usersfile);
        usersfile.close();
        response.set_content(usersdata.dump(), "application/json");
    });

    srv.listen("localhost", 43243);
    return 0;
}
