#include <iostream>
#include <httplib.h>
#include <json.hpp>
#include "auth/auth.hpp"
#include "users/users.hpp"
#include "colors.h"

#define CORS_ENABLE

#include "security/cors.hpp"

int main() {
    httplib::Server srv;

    srv.Post("/api/token", auth);
    srv.Post("/api/users/user", get_user);
    srv.Post("/api/users/all", get_all_users);
    srv.Post("/api/users/add", add_user);
    srv.Post("/api/users/user/password/change", change_password);
    srv.Post("/api/users/del", delete_user);
    srv.Post("/api/access", user_access);
    srv.Post("/api/users/user/change", change_user);

    srv.Options("/(.*)", set_cors_headers);

    srv.set_mount_point("/admin", "../src/web");
    srv.set_mount_point("/js", "../src/web/js");
    srv.set_mount_point("/css", "../src/web/css");

    std::cout << GREEN << "Starting" << RESET << " server on [ " << YELLOW
            << "localhost" << RESET << ":" << CYAN << "43243" << RESET << " ]" << std::endl;
    srv.listen("localhost", 43243);
    return 0;
}
