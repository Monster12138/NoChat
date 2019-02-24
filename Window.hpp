#pragma once
#include <iostream>
#include <ncurses.h>
#include <unistd.h>
#include <string>

class Window
{
public:
    Window() 
    {
        initscr();
    }

    void DrawHeader()
    {
        int h = LINES * 0.2;
        int w = COLS;
        int y = 0;
        int x = 0;
        header_ = newwin(h, w, y, x);
        box(header_, 0, 0);
        wrefresh(header_);
    }

    void DrawOutput()
    {
        int h = LINES * 0.6;
        int w = COLS * 0.75;
        int y = LINES * 0.2;
        int x = 0;
        output_ = newwin(h, w, y, x);
        box(output_, 0, 0);
        wrefresh(output_);
    }

    void DrawOnline()
    {
        int h = LINES * 0.6;
        int w = COLS * 0.25;
        int y = LINES * 0.2;
        int x = COLS * 0.75;
        online_ = newwin(h, w, y, x);
        box(online_, 0, 0);
        wrefresh(online_);
    }

    void DrawInput()
    {
        int h = LINES * 0.2;
        int w = COLS;
        int y = LINES * 0.8;
        int x = 0;
        input_ = newwin(h, w, y, x);
        box(input_, 0, 0);
        wrefresh(input_);
    }
    
    void Draw()
    {
        DrawHeader();
        DrawOutput();
        DrawOnline();
        DrawInput();
    }

    void GetStrFronWin(WINDOW *pw, std::string& str)
    {
    }

    void PutStrToWin(WINDOW *pw, int y, int x, const std::string& str)
    {
        mvwaddstr(pw, y, x, str.c_str());
        wrefresh(pw);
    }

    ~Window() 
    {
        endwin();
    }

    WINDOW *header_;
    WINDOW *output_;
    WINDOW *online_;
    WINDOW *input_;
};

