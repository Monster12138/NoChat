#ifndef _SERVER_NOCHAT_HPP_
#define _SERVER_NOCHAT_HPP_

#include "UserManager.hpp"
#include "ProtocolUtil.hpp"
#include "DataPool.hpp"
#include <pthread.h>
#include <thread>

void* HandlerRequest(void *arg);
class ServerNoChat;

class parm
{
public:
    ServerNoChat *sp_;
    int sock_;
    std::string ip_;
    int port_;
    parm(ServerNoChat *sp, int sock, const std::string &ip,const int port ):
        sp_(sp), 
        sock_(sock),
        ip_(ip),
        port_(port)
    {}
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

    unsigned int RegisterUser(const std::string& name, const std::string& passwd)
    {
        return um.Insert(name, passwd);
    }

    unsigned int LoginUser(const unsigned int & id, const std::string & passwd, const std::string &ip, const int port)
    {   
        int result =  um.Check(id, passwd);
        if(result >= ID_TRESHOLD)
        {
            //um.MoveToOnLine(id, ip, port);
        }
        return result;
    }

    void Start()
    {
        std::string ip;
        int port;
        for(;;)
        {
            int sock = SocketApi::Accept(tcp_listen_sock_, ip, port);
            parm tmp(this, sock, ip, port);
            if(sock > 0)
            {
                std::cout << "new client: " << ip << "-" << port << std::endl;
                pthread_t tid;
                pthread_create(&tid, NULL, HandlerRequest, (void*)&tmp);
            }
            
        }
    }

    void Producter()
    {
        std::string recvStr;
        Util::RecvMessage(udp_work_sock_, recvStr);
        //todo 
    }

    friend void* HandlerRequest(void *);
private:
    int tcp_listen_sock_;
    int tcp_port_;

    int udp_work_sock_;
    int udp_port_;
    
    UserManager um;
    DataPool dp;
};

void* HandlerRequest(void *arg)
{
    parm* pparm = (parm*)arg;
    ServerNoChat *sp = pparm->sp_;
    int sock = pparm->sock_;
    pthread_detach(pthread_self());

    Request rq;
    Util::RecvRequest(sock, rq);
    
    Json::Value root;
    Util::UnSeralizer(rq.text_, root);
    LOG(rq.text_, NORMAL);
    if(rq.method_ == "REGISTER")
    {
        std::string name = root["name"].asString();
        std::string passwd = root["passwd"].asString();
        unsigned int id = sp->RegisterUser(name, passwd);
        send(sock, &id, sizeof(id), 0);
    }
    else if(rq.method_ == "LOGIN")
    {
        unsigned int id = root["id"].asInt();
        std::string passwd = root["passwd"].asString();

        //check, move user to online
        unsigned int result = sp->LoginUser(id, passwd, pparm->ip_, pparm->port_);

        send(sock, &result, sizeof(result), 0);
    }
    else
    {
        
    }

    //recv
    //anys
    //response

    close(sock);
    return NULL;
}
#endif
