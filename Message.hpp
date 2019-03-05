#pragma once

#include <iostream>
#include <string>
#include "json/json.h"
#include "ProtocolUtil.hpp"

class Message
{
public:
    Message() {}

    Message(const std::string type,const std::string &name, unsigned int id, const std::string &text) :
        type_(type),
        nick_name_(name),
        id_(id),
        text_(text)
    {}

    ~Message() {}
    
    void ToSendString(std::string & sendString)
    {
        Json::Value root;
        root["type"] = type_;
        root["name"] = nick_name_;
        root["id"] = id_;
        root["text"] = text_;
        Util::Seralizer(root, sendString);
    }

    void ToRecvValue(std::string &recvString)
    {
        Json::Value root;
        Util::UnSeralizer(recvString, root);
        type_ = root["type"].asString();
        nick_name_ = root["name"].asString();
        id_ = root["id"].asInt();
        text_ = root["text"].asString();
    }

    std::string type_;
    std::string nick_name_;
    unsigned int id_;
    std::string text_;
};

