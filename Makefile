CXX=g++
CXXFLAGS=-Wall -l ssl -l crypto -I src/libs -I src/auth -I src/security -I src/users
SOURCES=src/auth_service.cpp src/auth/auth.cpp src/auth/verify_auth.cpp src/security/cors.cpp src/users/access_request.cpp src/users/add_user.cpp src/users/change_password.cpp src/users/change_user.cpp src/users/delete_user.cpp src/users/get_all_users.cpp src/users/get_user.cpp
TARGET=bin/auth_service

default: all

all: build

build:
	@if [ ! -d bin ]; then \
		mkdir bin; \
	fi
	$(CXX) -o $(TARGET) $(SOURCES) $(CXXFLAGS)

clean:
	@if [ -f $(TARGET) ]; then \
		rm -v $(TARGET); \
	fi
	