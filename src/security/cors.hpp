#ifndef CORS_HPP
#define CORS_HPP

#include <httplib.h>

#ifdef CORS_ENABLE

void set_cors_headers(const httplib::Request& request, httplib::Response& response);

#endif // CORS_ENABLE

#endif // CORS_HPP

