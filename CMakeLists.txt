cmake_minimum_required (VERSION 2.6)
project (auth_service)
set(CMAKE_CXX_STANDARD 11)

include_directories(./src/lib)

# find_package(cpp-httplib REQUIRED)

add_executable(auth_service src/auth_service.cpp)
# target_link_libraries(auth_service cpp-httplib)