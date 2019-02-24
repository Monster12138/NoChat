#include "ServerNoChat.hpp"

using  namespace std;

static void Usage(std::string proc)
{
    std::cout << "Usage: " << proc << " tcp_port udp_port" << std::endl;
}

void *RunProducter(void *arg)
{
    ServerNoChat *sp = (ServerNoChat*)arg;
    for(;;)
    {
        sp->Producter();
    }

    return sp;
}

void *RunConsume(void *arg)
{
    ServerNoChat* sp = (ServerNoChat* )arg;
    for(;;)
    {
        sp->Consumer();
    }

    return sp;
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

    thread product_th{RunProducter, sp};
    product_th.detach();
    thread consume_th{RunProducter, sp};
    consume_th.detach();
    sp->Start();


    return 0;
}
