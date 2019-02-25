#ifndef _PROTOCOL_UTIL_HPP_
#define _PROTOCOL_UTIL_HPP_

#include <iostream>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <string>
#include <unistd.h>
#include <sstream>
#include "Log.hpp"
#include "json/json.h"

const int BACKLOG = 5;
const int ID_TRESHOLD = 60000;

class Request
{
public:
    std::string method_;    //Register, Login, Logout
    std::string content_lenth_; 
    std::string blank_;
    std::string text_;

    Request():blank_("\n") {}
};

class Response
{
public:
    std::string status_;
    std::string content_lenth_; 
    std::string blank_;
    std::string text_;

    Response():blank_("\n") {}
};

class Util
{
public:
    static std::string IntToString(int x)
    {
        std::stringstream ss;
        ss << x;
        return ss.str();
    }

    static int StringToInt(std::string str)
    {
        std::stringstream ss(str);
        int x;
        ss >> x;
        return x;
    }

    static bool RegisterEnter(std::string& name, std::string &passwd)
    {

        std::cout << "Please Enter Nick Name: ";
        std::cin >> name;
        std::cout << "Please Enter Passwd: ";
        std::cin >> passwd;
        std::string AckPasswd;
        std::cout << "Please Enter Passwd Again: ";
        std::cin >> AckPasswd;

        if(passwd == AckPasswd)
        {
            return true;
        }

        return false;
    }

    static bool LoginEnter(unsigned int& id, std::string& passwd)
    {
        std::cout << "Please Enter Id: ";
        std::cin >> id;
        std::cout << "Please Enter Passwd: ";
        std::cin >> passwd;

        return true;
    }

    static  void Seralizer(Json::Value& root, std::string &outString)
    {
        Json::StreamWriterBuilder wBuilder;
        std::unique_ptr<Json::StreamWriter> writer(wBuilder.newStreamWriter());

        std::ostringstream os;        
        writer->write(root, &os);

        outString = os.str();
    }
    
    static bool UnSeralizer(std::string &inString, Json::Value &root)
    {
        Json::CharReaderBuilder rBuilder;
        std::unique_ptr<Json::CharReader> reader(rBuilder.newCharReader());
        int size = inString.size();
        JSONCPP_STRING errs;
        if(!reader->parse(inString.c_str(), inString.c_str() + size, &root, &errs))
        {
            return false;
        }
        return true;
    }

    static void RecvOneLine(int sock, std::string &outString)
    {
        char c = 'x';
        while(1)
        {
            ssize_t s = recv(sock, &c, 1 , 0);
            if(s > 0)
            {
                if(c == '\n')break;
                outString.push_back(c);
            }
            else
            {
                break;
            }
        }
    }

    static void SendReQuest(int sock, const Request &rq)
    {
        send(sock, rq.method_.c_str(), rq.method_.size(), 0);
        send(sock, rq.content_lenth_.c_str(), rq.content_lenth_.size(), 0);
        send(sock, rq.blank_.c_str(), rq.blank_.size(), 0);
        send(sock, rq.text_.c_str(), rq.text_.size(), 0);
    }
   

    static void RecvRequest(int sock, Request &rq)
    {
        RecvOneLine(sock, rq.method_);
        RecvOneLine(sock, rq.content_lenth_);
        RecvOneLine(sock, rq.blank_);
        std::string &cl = rq.content_lenth_;
        size_t pos = cl.find(": ");
        if(std::string::npos == pos )
        {
            LOG("Request msg's format error!", WARING);
            return;
        }
        std::string  lenstr = cl.substr(pos + 2);
        int size = StringToInt(lenstr);

        char c;
        for(auto i = 0; i < size; ++i)
        {
            recv(sock, &c, 1, 0);
            rq.text_.push_back(c);
        }
        
    }

    static void SendResponse(int sock, const Response &rp)
    {
        send(sock, rp.status_.c_str(), rp.status_.size(), 0);
        send(sock, rp.content_lenth_.c_str(), rp.content_lenth_.size(), 0);
        send(sock, rp.blank_.c_str(), rp.blank_.size(), 0);
        send(sock, rp.text_.c_str(), rp.text_.size(), 0);
    }

    static void RecvResponse(int sock, Response &rp)
    {
        RecvOneLine(sock, rp.status_);
        RecvOneLine(sock, rp.content_lenth_);
        RecvOneLine(sock, rp.blank_);
        std::string &cl = rp.content_lenth_;

        size_t pos = cl.find(": ");
        if(std::string::npos == pos)
        {
            LOG("Response msg's format error!", WARING);
            return;
        }

        std::string lenstr = cl.substr(pos + 2);
        int size = StringToInt(lenstr);

        char c;
        for(auto i = 0; i < size; ++i)
        {
            recv(sock, &c, 1, 0);
            rp.text_.push_back(c);
        }
    }

    static void RecvMessage(int sock, std::string& recvStr, struct sockaddr_in& clientAddr)
    {
        socklen_t len = sizeof(clientAddr);
        char msg[1024] = {0};
        ssize_t s = recvfrom(sock, msg, sizeof(msg) - 1, 0, (struct sockaddr *)&clientAddr, &len);
        if(s <= 0)
        {
            LOG("recvfrom message error", WARING);
        }
        else
        {
            recvStr = msg;
        }
    }

    static void SendMessage(int sock, const std::string& sendStr, const struct sockaddr_in& clientAddr )
    {
        sendto(sock, sendStr.c_str(), sendStr.size(), 0, (struct sockaddr*)&clientAddr, sizeof(clientAddr));
    }

};


class SocketApi
{
public:
    static int TcpSocket()
    {
        int sock = socket(AF_INET, SOCK_STREAM, 0);
        if(sock < 0)
        {
            LOG("socket error!", ERROR);
            exit(1);
        }
        return sock;
    }

    static int UdpSocket()
    {
        int sock = socket(AF_INET, SOCK_DGRAM, 0);
        if(sock < 0)
        {
            LOG("socket error!", ERROR);
            exit(1);
        }
        return sock;
    }

    static void Bind(int sock, int port)
    {
        struct sockaddr_in local;
        local.sin_family = AF_INET;
        local.sin_addr.s_addr = INADDR_ANY;
        local.sin_port = htons(port);

        if(bind(sock, (struct sockaddr*)&local, sizeof(local)) < 0)
        {
            LOG("socket error", ERROR);
            exit(2);
        }
    }
    
    static void Listen(int sock)
    {
        if(listen(sock, BACKLOG) < 0)
        {
            LOG("Listen error", ERROR);
            exit(3);
        }
    }

    static int Accept(int listen_sock, std::string &out_ip, int &out_port)
    {
        struct sockaddr_in peer;
        socklen_t len = sizeof(peer);
        int sock = accept(listen_sock, (struct sockaddr*)&peer, &len);
        if(sock < 0)
        {
            LOG("accept error", WARING);
            return -1;
        }

        out_ip = inet_ntoa(peer.sin_addr);
        out_port = ntohl(peer.sin_port);
        return sock;
    }

    static bool Connect(const int &sock, std::string &peer_ip, const int &port)
    {
        struct sockaddr_in peer;
        peer.sin_family = AF_INET;
        peer.sin_addr.s_addr = inet_addr(peer_ip.c_str());
        peer.sin_port = htons(port);

        socklen_t len = sizeof(peer);

        if(connect(sock, (struct sockaddr*)&peer, len) < 0)
        {
            LOG("connect error", WARING);
            return false;
        }
        return true;
    }

};

#endif
