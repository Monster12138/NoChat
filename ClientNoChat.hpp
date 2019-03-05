#ifndef _CLIENT_NOCHAT_HPP_
#define _CLIENT_NOCHAT_HPP_

#include "ProtocolUtil.hpp"
#include "Message.hpp"
#include "Window.hpp"
#include <iostream>
#include <string>
#include <list>
#include <fstream>
#include <unordered_map>
#include "pthread.h"
#include "BarragePool.hpp"
#include "UserManager.hpp"

const int TCP_PORT = 8666;
const int UDP_PORT = 8777;

void *RefreshMsg(void *);
void *Welcome(void *);
void *Output(void *);
class ClientNoChat;

struct cw_pair
{
    ClientNoChat *cp_;
    Window *wp_;

    cw_pair(ClientNoChat *cp, Window *wp):cp_(cp), wp_(wp) {} 
};

class ClientNoChat
{
public:
    ClientNoChat(const std::string ip):
        peer_ip_(ip), 
        tcp_sock_(-1), 
        udp_sock_(-1)
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
        if(Util::RegisterEnter(local_user_.nick_name_, local_user_.passwd_) && ConnectServer())
        {
            Request rq;
            rq.method_ = "REGISTER\n";

            Json::Value root;
            root["name"] = local_user_.nick_name_;
            root["passwd"] = local_user_.passwd_;
            
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
                local_user_.id_ = rpRoot["id"].asInt();
                std::cout << "Register Success! Your Login ID Is : " << local_user_.id_ << std::endl;
                return true;
            }
            else
            {
                std::cout << "Register Failed! Error number : " << local_user_.id_ << std::endl;
                return false;
            }
            close(tcp_sock_);
        }
        return false;
    }

    void GetOnlineUsers()
    {
        int i = 0, num;
        recv(tcp_sock_, &num, sizeof(int), 0);

        while(i++ < num)
        {
            int len;
            char buf[1024] = {0};
            std::string recvStr;
            recv(tcp_sock_, &len, sizeof(int), 0);
            recv(tcp_sock_, buf, len, 0);
            
            recvStr.assign(buf);
            Json::Value root;
            Util::UnSeralizer(recvStr, root);

            int id;
            std::string name;
            id = root["id"].asInt();
            name = root["name"].asString();

            onlineUsers_.push_back(User(name, id));
        }
    }

    bool Login()
    {
        if(Util::LoginEnter(local_user_.id_, local_user_.passwd_) && ConnectServer())
        {
            Request rq;
            rq.method_ = "LOGIN\n";

            Json::Value root;
            root["id"] = local_user_.id_;
            root["passwd"] = local_user_.passwd_;
            
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
                local_user_.nick_name_ = rpRoot["name"].asString();
                std::cout << "Login Success!\n Welcome " << local_user_.nick_name_ << std::endl;
                GetOnlineUsers();
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
        Window w;
        setlocale(LC_ALL,"");
        pthread_t head, output;
        cw_pair p(this, &w);        
        
        pthread_create(&head, 0, Welcome, &w);
        pthread_create(&output, 0, Output, &p);
        //pthread_create(&online, 0, Online, &w);
        
        SendOnlineMes();

        w.DrawInput();
        std::string text;
        for(;;)
        {
            w.GetStrFromInput(text);
            if(text == "")continue;
            std::string sendStr;
            Message msg("CHAT" ,local_user_.nick_name_, local_user_.id_, text);
            msg.ToSendString(sendStr);

            Util::SendMessage(udp_sock_, sendStr, server);        
        }

    }

    friend void *RefreshMsg(void *);
    friend void *Output(void *);
private:
    typedef std::list<User> OnlineUsers;
    std::string peer_ip_;
    int tcp_sock_;
    int udp_sock_;

    User local_user_;
    OnlineUsers onlineUsers_;

    struct sockaddr_in server;


    void SendOnlineMes()
    {
        std::string sendStr;
        Message msg("ONLINE" ,local_user_.nick_name_, local_user_.id_, "");
        msg.ToSendString(sendStr);

        Util::SendMessage(udp_sock_, sendStr, server);
    }
};

void *Welcome(void *arg)
{
    pthread_detach(pthread_self());
    Window *w = (Window *)arg;
    w->Welcome();

    return arg;
}

struct BPW_pair
{
    BarragePool *bpp_;
    Window *wp_;

    BPW_pair() {}
    BPW_pair(BarragePool *bpp, Window *wp):bpp_(bpp), wp_(wp) {}
};

void *ShowOutput(void *arg)
{
    BPW_pair* bpw_pair = (BPW_pair*) arg;
    Window *wp = bpw_pair->wp_;
    BarragePool *bp = bpw_pair->bpp_;
    
    std::list<Barrage> vec;
    
    wp->DrawOutput();
    int x, y;
    getmaxyx(wp->output_, y, x);
    delwin(wp->output_);

    std::ofstream fout("log.txt", std::ios::out);
    if(!fout.is_open())
    {
        exit(3);
    }
    for(;;)
    {
        while(!bp->Empty())
        {
            Barrage tmp;
            bp->GetBarrage(tmp);
            vec.push_front(tmp);
        }

        wp->DrawOutput();
        for(auto it = vec.begin(); it != vec.end(); ++it)
        {
            wp->PutStrToWin(wp->output_, (*it).rows_%(y - 2) + 1, (*it).cols_++, (*it).str_);
            if((*it).cols_ > (int)(x - (*it).str_.size() - 2))
            {
                vec.erase(it);
                break;
            }
        }
        delwin(wp->output_);

        usleep(80000);
    }

    return arg;
}

void *Output(void *arg)
{
    pthread_detach(pthread_self());
    cw_pair *cwpptr = (cw_pair*)arg;
    Window *wp = cwpptr->wp_;
    ClientNoChat *cp = cwpptr->cp_;

    static int rows = 0;
    BarragePool barragePool;
    BPW_pair bpw_pair(&barragePool, wp);
    pthread_t tid;
    pthread_create(&tid, 0, ShowOutput, (void*)&bpw_pair);

    std::string recvStr;
    
    for(;;)
    {
        Message msg;
        Util::RecvMessage(cp->udp_sock_, recvStr, cp->server);
        msg.ToRecvValue(recvStr);

        std::string showStr;
        showStr = msg.nick_name_ + ": " + msg.text_;
        
        barragePool.PutBarrage(Barrage(showStr, rows++));
    }
}


void ShowOnline()
{
    //todo        
}


#endif
