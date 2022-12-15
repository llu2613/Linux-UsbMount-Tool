#pragma once
#include <iostream>
#include <vector>
#include <thread>
#include <mutex>
#include <atomic>
#include <functional>
#include "SafeQueue.h"
#include <condition_variable>

class ThreadPool
{
    using Task = std::function<void()>;
public:
    ThreadPool(uint16_t threadNum = 4);
    ~ThreadPool();

    ThreadPool(const ThreadPool&) = delete;
    ThreadPool(ThreadPool&&) = delete;
    ThreadPool& operator=(const ThreadPool&) = delete;
    ThreadPool& operator=(ThreadPool&&) = delete;

    template<typename F, typename ...Args>
    void addTask(F&& task, Args&& ...arg) //万能引用
    {
        std::unique_lock<std::mutex> lk(lock);
        taskQueue.enqueue(std::bind(std::forward<F>(task), std::forward<Args>(arg)...));//函数和形参打包
        condition.notify_one();
    }
    bool takeTask(Task& task)
    {
        std::unique_lock<std::mutex> lk(lock);
        condition.wait(lk,[this](void)->bool{return !run || !taskQueue.empty();});
        if(taskQueue.empty())
        {
            return false;
        }
        taskQueue.dequeue(task);
        return true;
    }
    void workThread(int a)
    {
        std::cout << "thread " << a << " run" << std::endl;
        Task task;
        while(run)
        {
            if(takeTask(task))
            {
                freeThreadNum --;
                task();
                freeThreadNum ++;
            }
        }
        std::cout << "thread " << std::this_thread::get_id() << " exit" << std::endl;
    }
    void stop()
    {
        run = false;
    }
    uint16_t getFreeThreadNum()
    {
        return freeThreadNum;
    }
private:
    uint16_t initThreadNum;
    std::vector<std::thread> threadPool;
    SafeQueue<Task> taskQueue;
    std::mutex lock;
    std::condition_variable condition;
    std::atomic<bool> run;
    std::atomic<int> freeThreadNum;
};