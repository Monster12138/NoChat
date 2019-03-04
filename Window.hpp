#pragma once
#include <iostream>
#include <ncurses.h>
#include <unistd.h>
#include <string>
#include <mutex>
#include <list>
#include "BarragePool.hpp"


class Window
{
public:
    Window() 
    {
        initscr();
        curs_set(0);
    }

    void Safewrefresh(WINDOW* w)
    {
        locker_.lock();
        wrefresh(w);
        locker_.unlock();
    }

    void DrawHeader()
    {
        int h = LINES * 0.2;
        int w = COLS;
        int y = 0;
        int x = 0;
        header_ = newwin(h, w, y, x);
        box(header_, 0, 0);
        Safewrefresh(header_);
    }

    void DrawOutput()
    {
        int h = LINES * 0.6;
        int w = COLS * 0.75;
        int y = LINES * 0.2;
        int x = 0;
        output_ = newwin(h, w, y, x);
        box(output_, 0, 0);
        Safewrefresh(output_);
    }

    void DrawOnline()
    {
        int h = LINES * 0.6;
        int w = COLS * 0.25;
        int y = LINES * 0.2;
        int x = COLS * 0.75;
        online_ = newwin(h, w, y, x);
        box(online_, 0, 0);
        Safewrefresh(online_);
    }

    void DrawInput()
    {
        int h = LINES * 0.2;
        int w = COLS;
        int y = LINES * 0.8;
        int x = 0;
        input_ = newwin(h, w, y, x);
        box(input_, 0, 0);
        std::string tips = "Please Enter> ";
        PutStrToWin(input_, 2, 2, tips);
        Safewrefresh(input_);
    }
    
    void Draw()
    {
        DrawHeader();
        DrawOutput();
        DrawOnline();
        DrawInput();
    }

    void GetStrFromInput(std::string& str)
    {
        char buf[1024] = {0};
        wgetnstr(input_, buf, 1024);
        str = buf;
        
        delwin(input_);
        DrawInput();
    }

    void PutStrToWin(WINDOW *pw, int y, int x, const std::string& str)
    {
        mvwaddstr(pw, y, x, str.c_str());
        Safewrefresh(pw);
    }

    void Welcome()
    {
        std::string welcome = "welcome to NoChat!";
        unsigned int cols = 1, y, x;
        int dir = 0;
        for(;;)
        {
            getmaxyx(header_, y, x);
            DrawHeader();
            if(cols > x - welcome.size() -3)
                dir = 1;
            if(cols < 2)
                dir = 0;
            PutStrToWin(header_, y/2, cols, welcome);
            if(0 == dir)
            {
                ++cols;
            }
            else
            {
                --cols;
            }

            usleep(50000);
            delwin(header_);
        }
    }

    ~Window() 
    {
        endwin();
    }

    WINDOW *header_;
    WINDOW *output_;
    WINDOW *online_;
    WINDOW *input_;
    std::mutex locker_;
};


