#include <AuthService/AuthService.hpp>
#include <auth/auth.hpp>
#include <users/users.hpp>
#include <security/cors.hpp>
#include <colors.h>

AuthService::AuthService()
{
    this->srv.Post("/api/token", auth);
    this->srv.Post("/api/users/user", get_user);
    this->srv.Post("/api/users/all", get_all_users);
    this->srv.Post("/api/users/add", add_user);
    this->srv.Post("/api/users/user/password/change", change_password);
    this->srv.Post("/api/users/del", delete_user);
    this->srv.Post("/api/access", user_access);
    this->srv.Post("/api/users/user/change", change_user);

    this->srv.Options("/(.*)", set_cors_headers);

    this->srv.set_mount_point("/auth/admin", "../src/web");
    this->srv.set_mount_point("/auth/js", "../src/web/js");
    this->srv.set_mount_point("/auth/css", "../src/web/css");
}

AuthService::~AuthService() = default;

int AuthService::serve(const std::string &ip, int port, int socket_flags)
{
    std::cout << GREEN << "Starting" << RESET << " server on [ " << YELLOW
              << ip << RESET << ":" << CYAN << port << RESET << " ]" << std::endl;
    this->srv.listen(ip, port, socket_flags);
    return 0;
}
