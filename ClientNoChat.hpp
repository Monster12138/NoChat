#ifndef _CLIENT_NOCHAT_HPP_
#define _CLIENT_NOCHAT_HPP_

#include "ProtocolUtil.hpp"
#include "Message.hpp"
#include <iostream>
#include <string>

const int TCP_PORT = 8666;
const int UDP_PORT = 8777;

void *RefreshMsg(void *);


class ClientNoChat
{
public:
    ClientNoChat(const std::string ip):
        peer_ip_(ip), 
        tcp_sock_(-1), 
        udp_sock_(-1), 
        id_(0)
    {
        server.sin_family = AF_INET;
        server.sin_port = htons(UDP_PORT);
        server.sin_addr.s_addr = inet_addr(peer_ip_.c_str());
    }

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
            
            Response rp;
            Util::RecvResponse(tcp_sock_, rp);
            std::cout << rp.status_ << std::endl;
            std::cout << rp.content_lenth_ << std::endl;
            std::cout << rp.blank_ << std::endl;
            std::cout << rp.text_ << std::endl;
            if(rp.status_ == "SUCCESS")
            {
                Json::Value rpRoot;
                Util::UnSeralizer(rp.text_, rpRoot);
                id_ = rpRoot["id"].asInt();
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
            
            Response rp;
            Util::RecvResponse(tcp_sock_, rp);
            if(rp.status_ == "SUCCESS")
            {
                Json::Value rpRoot;
                Util::UnSeralizer(rp.text_, rpRoot);
                nick_name_ = rpRoot["name"].asString();
                std::cout << "Login Success!\n Welcome " << nick_name_ << std::endl;
                return true;
            }
            else
            {
                std::cout << "Login Failed : " << rp.status_ << std::endl;
                return false;
            }
            close(tcp_sock_);
        }
        return false;
    }

    void Chat()
    {
        pthread_t tid;
        pthread_create(&tid, 0, RefreshMsg, this);

        for(;;)
        {
            std::string text;
            std::cin >> text;
            Message msg(nick_name_, id_, text);

            std::string sendStr;
            msg.ToSendString(sendStr);
            Util::SendMessage(udp_sock_, sendStr, server);
        }
    }

    friend void *RefreshMsg(void *);
private:
    std::string peer_ip_;
    int tcp_sock_;
    int udp_sock_;

    unsigned int id_;
    std::string passwd_;
    std::string nick_name_;

    struct sockaddr_in server;
};

void *RefreshMsg(void *arg)
{
    pthread_detach(pthread_self());
    Message msg;
    ClientNoChat *cp = (ClientNoChat*)arg;

    for(;;)
    {
        std::string recvStr;
        Util::RecvMessage(cp->udp_sock_, recvStr, cp->server);
        msg.ToRecvValue(recvStr);
        std::cout <<  msg.nick_name_ << ":" << msg.text_ << std::endl;
    }
    return arg;
}
#endif
