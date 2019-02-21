#ifndef _SERVER_NOCHAT_HPP_
#define _SERVER_NOCHAT_HPP_

#include "ProtocolUtil.hpp"
#include <pthread.h>
#include <thread>

void* HandlerRequest(void *arg);
class ServerNoChat;

class parm
{
public:
    ServerNoChat *sp_;
    int sock_;
    parm(ServerNoChat *sp, int sock):sp_(sp), sock_(sock) {}
};

class ServerNoChat
{
public:
    ServerNoChat(int tcp_port = 8666, int udp_port = 8777):
        tcp_listen_sock_(-1),
        tcp_port_(tcp_port),
        udp_work_sock_(-1),
        udp_port_(udp_port)
    {}

    void InitServer()
    {
        tcp_listen_sock_ = SocketApi::TcpSocket();
        SocketApi::Bind(tcp_listen_sock_, tcp_port_);

        udp_work_sock_ = SocketApi::UdpSocket();
        SocketApi::Bind(udp_work_sock_, udp_port_);

        SocketApi::Listen(tcp_listen_sock_);

    }

    void Start()
    {
        std::string ip;
        int port;
        for(;;)
        {
            int sock = SocketApi::Accept(tcp_listen_sock_, ip, port);
            parm tmp(this, sock);
            if(sock > 0)
            {
                std::cout << "new client: " << ip << "-" << port << std::endl;
                pthread_t tid;
                pthread_create(&tid, NULL, HandlerRequest, (void*)&tmp);
            }
            
        }
    }

    friend void* HandlerRequest(void *);
private:
    int tcp_listen_sock_;
    int tcp_port_;

    int udp_work_sock_;
    int udp_port_;
    
};

void* HandlerRequest(void *arg)
{
    parm* pparm = (parm*)arg;
    ServerNoChat *sp = pparm->sp_;
    int sock = pparm->sock_;
    pthread_detach(pthread_self());

    //recv
    //anys
    //response

    close(sock);
    return NULL;
}
#endif
