#include "ClientNoChat.hpp"

using namespace std;
void Usage(string proc)
{
    cout << "Usage: " << proc << " peer_ip" << endl;
}

static void Menu(int& select)
{
    cout << "****************************************\n";
    cout << "****1.Regisiter           2.Login*******\n";
    cout << "****                      3.Exit *******\n";
    cout << "****************************************\n";
    cout << "****                  Please Select*****\n";
    cout << "****************************************\n";
    cin >> select;
}

int main(int argc, char **argv)
{
    if(argc != 2)
    {
        Usage(argv[0]);
    }
    ClientNoChat *cp = new ClientNoChat(argv[1]); 
    cp->InitClient();
    int select = 0;
    Menu(select);
    for(;;)
    {
        switch(select)
        {
        case 1:
            cp->Register();
            break;
        case 2:
            if(cp->Login())
            {
                cp->Chat();
            }
            break;
        case 3:
            exit(0);
            break;
        default:
            break;
        }

    }
    return 0;
}
