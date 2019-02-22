#pragma once
#include <iostream>
#include <string>
#include <unordered_map>
#include <vector>
#include <arpa/inet.h>

class User
{
public:
    User(const std::string& nick_name, const unsigned int& id, const std::string & passwd):
        nick_name_(nick_name),
        id_(id),
        passwd_(passwd)
    {}
    std::string nick_name_;
    unsigned int id_;
    std::string passwd_;
};

class UserManager
{
public:
    unsigned int assign_id;
    typedef std::unordered_map<unsigned int, User> UserMap;
    typedef std::unordered_map<unsigned int, struct sockaddr_in> OnlineUser;
    UserManager() {}
    ~UserManager() {}

private:
    UserMap users;
    OnlineUser onlineUsers;
};

