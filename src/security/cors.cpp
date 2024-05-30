#include "cors.hpp"
#include "../colors.h"

void set_cors_headers(const httplib::Request& request, httplib::Response& response) {
    std::cout << GREEN << request.path << "  " << std::endl;
    response.set_header("Access-Control-Allow-Origin", "*");
    response.set_header("Access-Control-Allow-Methods", "GET,POST,OPTIONS");
    response.set_header("Access-Control-Allow-Headers", "Authorization,X-Custom,Content-Type");
}

