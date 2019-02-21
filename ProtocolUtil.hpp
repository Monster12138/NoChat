#ifndef _PROTOCOL_UTIL_HPP_
#define _PROTOCOL_UTIL_HPP_

#include <iostream>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <string>
#include <unistd.h>
#include "Log.hpp"

const int BACKLOG = 5;

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
