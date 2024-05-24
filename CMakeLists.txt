cmake_minimum_required (VERSION 2.6)
project(auth_service)
set(CMAKE_CXX_STANDARD 17)

include_directories(./src/libs/)

find_package(jwt-cpp REQUIRED)

add_executable(
    auth_service 
    src/auth_service.cpp 

    src/auth/auth.cpp
    src/auth/verify_auth.cpp 
    
    src/users/access_request.cpp
    src/users/change_password.cpp
    src/users/delete_user.cpp
    src/users/add_user.cpp
    src/users/get_all_users.cpp
    src/users/get_user.cpp
)
target_link_libraries(auth_service jwt-cpp::jwt-cpp)
