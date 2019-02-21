#pragma once
#include <iostream>
#include <string>

const int NORMAL = 0;
const int WARING = 1;
const int ERROR = 2;

const char* level_2_str[] = {
    "Normal",
    "Warning",
    "Error",
    nullptr
};

void Log(const std::string& msg, int level, const std::string file, int line)
{
    std::cout << '[' << msg << ']' << "-[" << level_2_str[level] << ']'  
        << ':' << file << ':' << line << std::endl; 
}

#define LOG(msg, level) Log(msg, level, __FILE__, __LINE__) 
