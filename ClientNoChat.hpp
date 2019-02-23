#ifndef _CLIENT_NOCHAT_HPP_
#define _CLIENT_NOCHAT_HPP_

#include "ProtocolUtil.hpp"
#include <iostream>
#include <string>

const int TCP_PORT = 8666;
const int UDP_PORT = 8777;
class ClientNoChat
{
public:
    ClientNoChat(const std::string ip):
        peer_ip_(ip), 
        tcp_sock_(-1), 
        udp_sock_(-1), 
        id_(0)
    {}

    void InitClient()
    {
        udp_sock_ = SocketApi::UdpSocket();
    }

    void Start()
    {
        std::cout << peer_ip_ << ":" << TCP_PORT << std::endl;
        if(SocketApi::Connect(tcp_sock_, peer_ip_, TCP_PORT))
        {
            std::cout << "Connect success" << std::endl;
            //send
            //recv
        }
    }
    
    bool ConnectServer()
    {
        tcp_sock_ = SocketApi::TcpSocket();
        return SocketApi::Connect(tcp_sock_, peer_ip_, TCP_PORT);
    }

    bool Register()
    {
        if(Util::RegisterEnter(nick_name_, passwd_) && ConnectServer())
        {
            Request rq;
            rq.method_ = "REGISTER\n";

            Json::Value root;
            root["name"] = nick_name_;
            root["passwd"] = passwd_;
            
            Util::Seralizer(root, rq.text_);
            rq.content_lenth_ = "Content-Length: ";
            rq.content_lenth_ += Util::IntToString(rq.text_.size());
            rq.content_lenth_ += '\n';

            Util::SendReQuest(tcp_sock_, rq);
            
            recv(tcp_sock_, &id_, sizeof(id_), 0);
            if(id_ >= ID_TRESHOLD)
            {
                std::cout << "Register Success! Your Login ID Is : " << id_ << std::endl;
                return true;
            }
            else
            {
                std::cout << "Register Failed! Error number : " << id_ << std::endl;
                return false;
            }
            close(tcp_sock_);
        }
        return false;
    }

    bool Login()
    {
        if(Util::LoginEnter(id_, passwd_) && ConnectServer())
        {
            Request rq;
            rq.method_ = "LOGIN\n";

            Json::Value root;
            root["id"] = id_;
            root["passwd"] = passwd_;
            
            Util::Seralizer(root, rq.text_);
            rq.content_lenth_ = "Content-Length: ";
            rq.content_lenth_ += Util::IntToString(rq.text_.size());
            rq.content_lenth_ += '\n';

            Util::SendReQuest(tcp_sock_, rq);
            
            unsigned int result;
            recv(tcp_sock_, &result, sizeof(result), 0);
            if(id_ >= ID_TRESHOLD)
            {
                std::cout << "Login Success!" << std::endl;
                return true;
            }
            else
            {
                std::cout << "Login Failed! Error number : " << result << std::endl;
                return false;
            }
            close(tcp_sock_);
        }
        return false;
    }

    void Chat()
    {

    }
private:
    std::string peer_ip_;
    int tcp_sock_;
    int udp_sock_;

    unsigned int id_;
    std::string passwd_;
    std::string nick_name_;
};

#endif
