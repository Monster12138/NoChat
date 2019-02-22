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
        tcp_sock_ = SocketApi::TcpSocket();
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
        return SocketApi::Connect(tcp_sock_, peer_ip_, TCP_PORT);
    }

    void Register()
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

            SocketApi::Send(tcp_sock_, rq);
        }
    }

    bool Login()
    {
        
        return true;
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
