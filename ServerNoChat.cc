#include "ServerNoChat.hpp"

static void Usage(std::string proc)
{
    std::cout << "Usage: " << proc << " tcp_port udp_port" << std::endl;
}

int main(int argc, char **argv)
{
    if(argc != 3)
    {
        Usage(argv[0]);
        exit(1);
    }

    int tcp_port = atoi(argv[1]);
    int udp_port = atoi(argv[2]);

    ServerNoChat *sp = new ServerNoChat(tcp_port, udp_port);
    sp->InitServer();
    sp->Start();


    return 0;
}
