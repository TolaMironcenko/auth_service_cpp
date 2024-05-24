#include "auth.hpp"

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