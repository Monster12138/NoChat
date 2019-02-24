#include <iostream>
#include "Window.hpp"

int main()
{
    Window w;
    w.Draw();
    w.PutStrToWin(w.header_, 2, 3, "hello world!"); 

    getchar();
    return 0;
}

