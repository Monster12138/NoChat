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
    ClientNoChat(const std::string ip):peer_ip_(ip), tcp_sock_(-1), udp_sock_(-1) {}

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
private:
    std::string peer_ip_;
    int tcp_sock_;
    int udp_sock_;
};

#endif
