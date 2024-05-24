#include <iostream>
#include <fstream>
#include <httplib.h>
#include <json.hpp>
#include <jwt-cpp/jwt.h>

#include "auth/auth.hpp"
#include "users/users.hpp"

int main() {
    httplib::Server srv;

    srv.Post("/api/token", auth);
    srv.Post("/api/users/user", get_user);
    srv.Post("/api/users/all", get_all_users);
    srv.Post("/api/users/add", add_user);
    srv.Post("/api/users/user/password/change", change_password);
    srv.Post("/api/users/del", delete_user);
    srv.Post("/api/access", user_access);

    srv.set_mount_point("/admin", "../src/web");
    srv.set_mount_point("/js", "../src/web/js");
    srv.set_mount_point("/css", "../src/web/css");

    srv.listen("localhost", 43243);
    return 0;
}
