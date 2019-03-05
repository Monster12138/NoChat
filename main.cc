#include <iostream>
#include "Window.hpp"

int main()
{
    Window w;
    w.Draw();
    for(;;)
    {
        std::string str;
        w.GetStrFromInput( str);
    }
    getchar();
    return 0;
}

