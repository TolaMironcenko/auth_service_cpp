#include <iostream>
#include "libs/httplib.h"
#include "libs/json.hpp"
#include <fstream>

int main() {
    httplib::Server srv;

    srv.Get("/api/token", [](const httplib::Request& request, httplib::Response& response) {
        response.set_content("{\"token\":\"qwerty\"}", "application/json");
    });

    srv.Get("/api/users", [](const httplib::Request& request, httplib::Response& response) {
        std::ifstream usersfile("users.json");
        nlohmann::json usersdata = nlohmann::json::parse(usersfile);
        usersfile.close();
        response.set_content(usersdata.dump(), "application/json");
    });

    srv.listen("localhost", 43243);
    return 0;
}
