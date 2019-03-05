#ifndef _SERVER_NOCHAT_HPP_
#define _SERVER_NOCHAT_HPP_

#include "UserManager.hpp"
#include "ProtocolUtil.hpp"
#include "DataPool.hpp"
#include "Message.hpp"
#include <pthread.h>

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
        int opt;

        tcp_listen_sock_ = SocketApi::TcpSocket();
        setsockopt(tcp_listen_sock_, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));
        SocketApi::Bind(tcp_listen_sock_, tcp_port_);

        udp_work_sock_ = SocketApi::UdpSocket();
        setsockopt(udp_work_sock_, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));
        SocketApi::Bind(udp_work_sock_, udp_port_);

        SocketApi::Listen(tcp_listen_sock_);

    }

    unsigned int RegisterUser(const std::string& name, const std::string& passwd)
    {
        return um.Insert(name, passwd);
    }

    Response LoginUser(const unsigned int & id, const std::string & passwd)
    {   
        unsigned int result = um.Check(id, passwd);
        Response rp;
        if(result < ID_TRESHOLD)
        {
            if(result == 1)
                rp.status_ = "User does not exist\n";
            else
                rp.status_ = "Wrong password\n";
            rp.content_lenth_ = "Content-length: 0\n";
            rp.text_ = "";
        }
        else
        {
            rp.status_ = "SUCCESS\n";
            rp.content_lenth_ = "Content-length: ";

            User thisUser = um.GetUserInfo(result);
            Json::Value root;
            root["name"] = thisUser.nick_name_;

            Util::Seralizer(root, rp.text_);

            rp.content_lenth_ += Util::IntToString(rp.text_.size());
            rp.content_lenth_ += '\n';
        }
        return rp;
    }

    void Start()
    {
        std::string ip;
        int port;
        for(;;)
        {
            int opt;
            int sock = SocketApi::Accept(tcp_listen_sock_, ip, port);
            setsockopt(sock, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));
            parm tmp(this, sock, ip, port);
            if(sock > 0)
            {
                std::cout << "new client: " << ip << "-" << port << std::endl;
                pthread_t tid;
                pthread_create(&tid, 0, HandlerRequest, &tmp);
            }
            
        }
    }

    void Producter()
    {
        std::string recvStr;
        struct sockaddr_in clientAddr;
        Util::RecvMessage(udp_work_sock_, recvStr, clientAddr);
        if(!recvStr.empty())
        {
            Message m;
            m.ToRecvValue(recvStr);
            //std::cout << "recv Msg: " << recvStr << std::endl;
            if(m.type_ == "ONLINE")
                um.AddOnlineUser(m.id_, clientAddr);
            dp.PutMessage(recvStr);        
        }
    }

    void Consumer()
    {
        std::string msg;
        dp.GetMessage(msg);
        std::cout << "get msg: " << msg << std::endl;

        auto online = um.GetOnlineUsers();
        for(auto it = online.begin(); it != online.end(); ++it)
        {
            Util::SendMessage(udp_work_sock_, msg, it->second);
            std::cout << "send Msg: " << msg << std::endl;
        }
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

void SendOnlineUsers(int sock, UserManager &um)
{
    auto onlineUsers = um.GetOnlineUsers();
    int num = onlineUsers.size();
    send(sock, &num, sizeof(int), 0);

    for(auto it = onlineUsers.begin(); it != onlineUsers.end(); ++it)
    {
        Json::Value root;
        root["id"] = (*it).first;
        root["name"] = um.GetUserInfo((*it).first).nick_name_;

        std::string sendStr;
        Util::Seralizer(root, sendStr);
     
        int len = sendStr.size();
        send(sock, &len, sizeof(int), 0);
        send(sock, sendStr.c_str(), sendStr.size(), 0);
        std::cout << "send onlneuser: " << sendStr << std::endl;
    }
}

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

        Response rp;
        rp.status_ = "SUCCESS\n";
        Json::Value root;
        root["id"] = id;

        Util::Seralizer(root, rp.text_);
        rp.content_lenth_ = "Content-length: ";
        rp.content_lenth_ += Util::IntToString(rp.text_.size());
        rp.content_lenth_ += '\n';

        Util::SendResponse(sock, rp);
    }
    else if(rq.method_ == "LOGIN")
    {
        unsigned int id = root["id"].asInt();
        std::string passwd = root["passwd"].asString();

        //check, move user to online
        Response rp = sp->LoginUser(id, passwd);
        
        Util::SendResponse(sock, rp);

        if(rp.status_ == "SUCCESS")
        {
            SendOnlineUsers(sock, sp->um);
        }
    }
    else
    {
        
    }

    close(sock);
    return NULL;
}
#endif
