#include "ClientNoChat.hpp"

using namespace std;
void Usage(string proc)
{
    cout << "Usage: " << proc << " peer_ip" << endl;
}

int main(int argc, char **argv)
{
    if(argc != 2)
    {
        Usage(argv[0]);
    }
    
    ClientNoChat *cp = new ClientNoChat(argv[1]); 
    cp->InitClient();
    cp->Start();
    return 0;
}
