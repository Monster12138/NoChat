#pragma once
#include <semaphore.h>
#include <vector>
#include <string>

class DataPool
{
public:
    DataPool() {}
    ~DataPool() {}
    
    std::vector<std::string> pool_;
    int cap_;
    sem_t data_sem_;
    sem_t blank_sem_;
};

