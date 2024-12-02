#include <auth/auth.hpp>
#include <json.hpp>
#include <includes.hpp>
#include <jwt.hpp>
#include <syslog.h>

// function for verify tokens
bool verify_auth(const std::string &token) {
    try {
        return JWT::verifyJWT(token, JWT_SECRET_KEY);
    } catch (const std::system_error &e) {
        std::cout << "Verification error: " << e.what() << std::endl;
        syslog(LOG_ERR, "token verification error");
        return false;
    }
    catch (...) {
        return false;
    }
}
