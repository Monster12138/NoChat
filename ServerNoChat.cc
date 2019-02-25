#include "ServerNoChat.hpp"

using  namespace std;

static void Usage(std::string proc)
{
    std::cout << "Usage: " << proc << " tcp_port udp_port" << std::endl;
}

void* RunProducter(void *arg)
{
    pthread_detach(pthread_self());
    std::cout << "Producter thread run\n";
    ServerNoChat *sp = (ServerNoChat*)arg;
    for(;;)
    {
        sp->Producter();
    }
    
    return sp;
}

void* RunConsume(void *arg)
{
    pthread_detach(pthread_self());
    std::cout << "Consume thread run\n";
    ServerNoChat* sp = (ServerNoChat*)arg;
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

    pthread_t tid;
    pthread_create(&tid, 0, RunProducter, sp);
    pthread_create(&tid, 0, RunConsume, sp);
    sp->Start();


    return 0;
}
