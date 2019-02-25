#pragma once
#include <iostream>
#include <string>
#include <unordered_map>
#include <vector>
#include <arpa/inet.h>
#include <mutex>

class User
{
public:
    User() {}

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
    std::mutex lock_;
    typedef std::unordered_map<unsigned int, User> UserMap;
    typedef std::unordered_map<unsigned int, struct sockaddr_in> OnlineUser;

    UserManager():assign_id(60000) {}
    ~UserManager() {}
    
    unsigned int Insert(const std::string &name, const std::string &passwd)
    {
        lock_.lock();
        unsigned int id = assign_id++;
        User newuser(name, id, passwd);
        if(users.find(id) == users.end())
        {
            users.insert(std::make_pair(id, newuser));
            lock_.unlock();
            return id;
        }

        lock_.unlock();
        return 1;
    }

    unsigned int Check(const unsigned int & id, const std::string & passwd)
    {
        lock_.lock();
        auto it = users.find(id);
        if(it != users.end())
        {
            if(it->second.passwd_  == passwd)
            {
                lock_.unlock();
                return id;
            }
            else
                return 2;
        }

        lock_.unlock();
        return 1;
    }

    void AddOnlineUser(const unsigned int id, struct sockaddr_in & clientAddr)
    {
        lock_.lock ();
        auto it = onlineUsers.find(id);
        if(it == onlineUsers.end())
        {
            onlineUsers.insert({id, clientAddr});
        }

        lock_.unlock();
    }

    OnlineUser GetOnlineUsers()
    {
        lock_.lock();
        auto online = onlineUsers;
        lock_.unlock();
        return online;
    }

    User GetUserInfo(unsigned int id)
    {
        return users[id];
    }
private:
    UserMap users;
    OnlineUser onlineUsers;
};

