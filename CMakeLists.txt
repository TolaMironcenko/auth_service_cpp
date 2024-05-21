cmake_minimum_required (VERSION 2.6)
project (auth_service)
set(CMAKE_CXX_STANDARD 11)

include_directories(./src/libs/)

find_package(jwt-cpp REQUIRED)

add_executable(auth_service src/auth_service.cpp src/auth/auth.cpp)
target_link_libraries(auth_service jwt-cpp::jwt-cpp)