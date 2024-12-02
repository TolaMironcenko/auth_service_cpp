#include <AuthService/AuthService.hpp>

int main() {
    AuthService auth_service;
    return auth_service.serve("127.0.0.1", 43243);
}
