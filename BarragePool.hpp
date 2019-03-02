#pragma once

#include <semaphore.h>
#include <vector>
#include <string>
#include <iostream>

class Barrage
{
public:
    Barrage() {}
    Barrage(const std::string &str, int rows):str_(str), cols_(1), rows_(rows) {}

    std::string str_;
    int cols_;
    int rows_;
};

class BarragePool
{
public:
    BarragePool(int cap = 1024):
        pool_(cap),
        cap_(cap),
        product_step_(0),
        consume_step_(0)
    {
        sem_init(&data_sem_, 0, 0);
        sem_init(&blank_sem_, 0, cap);
    }

    ~BarragePool() 
    {
        sem_destroy(&data_sem_);
        sem_destroy(&blank_sem_);
    }
    
    void PutBarrage(const Barrage &b)
    {
        sem_wait(&blank_sem_);
        pool_[product_step_++] = b;
        product_step_ %= cap_; 
        sem_post(&data_sem_);
    }

    void GetBarrage(Barrage &b)
    {
        sem_wait(&data_sem_);
        b = pool_[consume_step_++];
        consume_step_ %= cap_;
        sem_post(&blank_sem_);
    }

    bool Empty()
    {
        return product_step_ == consume_step_;
    }
private:
    std::vector<Barrage> pool_;
    int cap_;
    sem_t data_sem_;
    sem_t blank_sem_;
    int product_step_;
    int consume_step_;
};

