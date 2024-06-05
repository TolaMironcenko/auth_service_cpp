#ifndef INCLUDES_HPP
#define INCLUDES_HPP

#define JWT_SECRET_KEY "secret"
#define STRING403 R"({"status":"403"})"
#define JSON_TYPE "application/json"
#define AUTH_REQUIRED_STRING R"({"required":"[username,password]"})"
#define ACCESS_REQUIRED_STRING R"({"required":"[token]"})"
#define ADD_USER_REQUIRED_STRING R"({"required":"[token,username,password]","optional":"[group,is_superuser]"})"
#define CHANGE_PASSWORD_REQUIRED_STRING R"({"required":"[token,old_password,new_password] or if you superuser [token,userid,new_password]"})"
#define CHANGE_USER_REQUIRED_STRING R"({"required":"[token] or if you superuser [token,userid]","optional":"[username,password,is_superuser,group]"})"
#define DELETE_USER_REQUIRED_STRING R"({"required":"[token] or if you superuser [token,userid]"})"

#endif // INCLUDES_HPP
