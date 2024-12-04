#pragma once

#include <iostream>
#include <string>
#include <openssl/sha.h>
#include <iomanip>
#include <sstream>

// funcion for hashing password using sha256
std::string hashPassword(const std::string &password) {
    unsigned char hash[SHA256_DIGEST_LENGTH];

    // calculating hash
    SHA256(reinterpret_cast<const unsigned char*>(password.c_str()), password.size(), hash);

    // reinterpreter as hex string
    std::stringstream ss;
    for (int i = 0; i < SHA256_DIGEST_LENGTH; ++i) {
        ss << std::setw(2) << std::setfill('0') << std::hex << static_cast<int>(hash[i]);
    }

    return ss.str();
}
