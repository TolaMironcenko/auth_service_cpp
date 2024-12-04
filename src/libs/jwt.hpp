#pragma once

#include <iostream>
#include <string>
#include <ctime>
#include <sstream>
#include <iomanip>
#include <openssl/hmac.h>
#include <openssl/buffer.h>
#include <openssl/evp.h>
#include <vector>
#include <openssl/bio.h>
#include <json.hpp>

#define JWT_DEFAULT_SECRET "secret"

namespace JWT {
    inline std::string base64_encode(const unsigned char *input, int length);
    inline std::string base64_decode(const std::string &in);
    inline std::string createJWT(const std::string &header_json, const std::string &payload_json, const std::string &secret);
    inline std::string hmacSha256(const std::string &key, const std::string &data);
    inline bool verifyJWT(const std::string &token, const std::string &secret);
    inline nlohmann::json get_payload(std::string token, std::string secret);
    inline nlohmann::json get_header(std::string token, std::string secret);
    // Function to Base64 Encode
    inline std::string base64_encode(const unsigned char *input, int length) {
        BIO *bio, *b64;
        BUF_MEM *bufferPtr;
        
        b64 = BIO_new(BIO_f_base64());
        bio = BIO_new(BIO_s_mem());
        bio = BIO_push(b64, bio);
        BIO_set_flags(bio, BIO_FLAGS_BASE64_NO_NL); // No new lines
        BIO_write(bio, input, length);
        BIO_flush(bio);
        BIO_get_mem_ptr(bio, &bufferPtr);
        
        std::string result(bufferPtr->data, bufferPtr->length);
        BIO_free_all(bio);
        return result;
    }

    inline std::string base64_decode(const std::string &in) {
        BIO *bio, *b64;
        BUF_MEM *bufferPtr;
        
        b64 = BIO_new(BIO_f_base64());
        bio = BIO_new_mem_buf(in.data(), in.size());
        bio = BIO_push(b64, bio);

        BIO_set_flags(bio, BIO_FLAGS_BASE64_NO_NL);
        BIO_get_mem_ptr(bio, &bufferPtr);
        BIO_set_close(bio, BIO_NOCLOSE);
        
        std::string out(bufferPtr->data, bufferPtr->length);
        BIO_reset(bio);
        
        std::vector<char> buffer(out.size());
        int decodedLength = BIO_read(bio, buffer.data(), buffer.size());
        BIO_free_all(bio);
        
        return std::string(buffer.data(), decodedLength);
    }

    // Function to encode JWT
    inline std::string createJWT(const std::string &header_json, const std::string &payload_json, const std::string &secret) {
        // Base64 URL Encode the header
        auto encodedHeader = base64_encode(reinterpret_cast<const unsigned char*>(header_json.c_str()), header_json.length());
        // Base64 URL Encode the payload
        auto encodedPayload = base64_encode(reinterpret_cast<const unsigned char*>(payload_json.c_str()), payload_json.length());
        
        // Prepare the signing input
        std::string unsignedToken = encodedHeader + "." + encodedPayload;
        
        // Sign the token
        unsigned char *digest;
        unsigned int digestLength;
        
        digest = HMAC(EVP_sha256(), secret.c_str(), secret.length(), 
                    reinterpret_cast<const unsigned char *>(unsignedToken.c_str()), unsignedToken.length(), 
                    nullptr, &digestLength);
        
        // Base64 URL Encode the signature
        std::string encodedSignature = base64_encode(digest, digestLength);
        
        // Form the final JWT token
        std::string jwt = unsignedToken + "." + encodedSignature;
        
        return jwt;
    }

    inline std::string hmacSha256(const std::string &key, const std::string &data) {
        unsigned char hash[EVP_MAX_MD_SIZE];
        unsigned int length = 0;

        HMAC(EVP_sha256(), key.c_str(), key.length(),
            reinterpret_cast<const unsigned char*>(data.c_str()), data.length(),
            hash, &length);

        return std::string(reinterpret_cast<char*>(hash), length);
    }

    inline bool verifyJWT(const std::string &token, const std::string &secret) {
        // Split the token into its three parts
        size_t pos1 = token.find('.');
        size_t pos2 = token.find('.', pos1 + 1);
        
        if (pos1 == std::string::npos || pos2 == std::string::npos) {
            std::cerr << "Invalid token format." << std::endl;
            return false;
        }

        std::string header = token.substr(0, pos1);
        std::string payload = token.substr(pos1 + 1, pos2 - pos1 - 1);
        std::string signature = token.substr(pos2 + 1);

        // Decode the header and payload
        std::string decodedHeader = base64_decode(header);
        std::string decodedPayload = base64_decode(payload);

        // Create the signature base string
        std::string signatureBase = header + "." + payload;

        // Calculate the expected signature
        std::string expectedSignature = base64_decode(signature);
        std::string calculatedSignature = hmacSha256(secret, signatureBase);

        // Verify the signature
        if (calculatedSignature != expectedSignature) {
            std::cerr << "Invalid signature." << std::endl;
            return false;
        }

        // Parse the payload as JSON to check claims
        auto jsonPayload = nlohmann::json::parse(decodedPayload, nullptr, false);
        if (jsonPayload.is_discarded()) {
            std::cerr << "Invalid JSON payload." << std::endl;
            return false;
        }

        // Check expiration
        if (jsonPayload.contains("exp")) {
            std::time_t exp = jsonPayload["exp"].get<std::time_t>();
            std::time_t now = std::time(nullptr);
            if (now >= exp) {
                std::cerr << "Token has expired." << std::endl;
                return false;
            }
        }

        return true;
    }

    inline nlohmann::json get_payload(std::string token, std::string secret) {
        size_t pos1 = token.find('.');
        size_t pos2 = token.find('.', pos1 + 1);
        
        if (pos1 == std::string::npos || pos2 == std::string::npos) {
            std::cerr << "Invalid token format." << std::endl;
            return false;
        }

        std::string payload = token.substr(pos1 + 1, pos2 - pos1 - 1);

        std::string decodedPayload = base64_decode(payload);
        nlohmann::json jsonPayload = nlohmann::json::parse(decodedPayload);
        return jsonPayload;
    }

    inline nlohmann::json get_header(std::string token, std::string secret) {
        size_t pos1 = token.find('.');
        size_t pos2 = token.find('.', pos1 + 1);
        
        if (pos1 == std::string::npos || pos2 == std::string::npos) {
            std::cerr << "Invalid token format." << std::endl;
            return false;
        }

        std::string header = token.substr(0, pos1);

        std::string decodedHeader = base64_decode(header);
        nlohmann::json jsonHeader = nlohmann::json::parse(decodedHeader);
        return jsonHeader;
    }
} // End of namespace JWT
