project('auth_service', 'cpp')
include_dirs = include_directories('src', 'src/libs')
deps = dependency('openssl')
executable(
    'auth_service', 
    'src/auth_service.cpp', 
    'src/users/access_request.cpp', 
    'src/users/add_user.cpp', 
    'src/users/change_password.cpp', 
    'src/users/change_user.cpp', 
    'src/users/delete_user.cpp', 
    'src/users/get_all_users.cpp', 
    'src/users/get_user.cpp', 
    'src/security/cors.cpp', 
    'src/AuthService/AuthService.cpp', 
    'src/auth/auth.cpp', 
    'src/auth/verify_auth.cpp', 
    include_directories : include_dirs, 
    dependencies : deps
)
