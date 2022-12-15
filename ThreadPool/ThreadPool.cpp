#include "ThreadPool.h"

ThreadPool::ThreadPool(uint16_t threadNum):initThreadNum(threadNum), run(true), freeThreadNum(0) 
{
    for(uint16_t i = 0; i < initThreadNum; i++)
    {
        threadPool.push_back(std::thread(&ThreadPool::workThread, this, freeThreadNum++));
        
    }
}
ThreadPool::~ThreadPool()
{
    run = false;
    condition.notify_all();
    for(auto& it : threadPool)
    {
        if(it.joinable())
        {
            it.join();
        }
    }
}