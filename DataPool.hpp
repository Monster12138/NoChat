#pragma once

#include <semaphore.h>
#include <vector>
#include <string>

class DataPool
{
public:
    DataPool(int cap = 1024):
        pool_(cap),
        cap_(cap),
        product_step_(0),
        consume_step_(0)
    {
        sem_init(&data_sem_, 0, 0);
        sem_init(&blank_sem_, 0, cap);
    }

    ~DataPool() 
    {
        sem_destroy(&data_sem_);
        sem_destroy(&blank_sem_);
    }
    
    void PutMessage(const std::string& msg)
    {
        sem_wait(&blank_sem_);
        pool_[product_step_++] = msg;
        product_step_ %= cap_; 
        sem_post(&data_sem_);
    }

    void GetMessage(std::string &msg)
    {
        sem_wait(&data_sem_);
        msg = pool_[consume_step_++];
        consume_step_ %= cap_;
        sem_post(&blank_sem_);
    }
private:
    std::vector<std::string> pool_;
    int cap_;
    sem_t data_sem_;
    sem_t blank_sem_;
    int product_step_;
    int consume_step_;
};

