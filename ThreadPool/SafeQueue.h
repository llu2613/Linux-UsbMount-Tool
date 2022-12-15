#pragma once
#include <queue>
#include <mutex>

template<typename T>
class SafeQueue
{
public:
    SafeQueue(){}
    SafeQueue(const SafeQueue&) = delete;
    SafeQueue(SafeQueue&&) = delete;
    SafeQueue& operator= (const SafeQueue&) = delete;
    SafeQueue& operator= (SafeQueue&&) = delete;
    ~SafeQueue(){}

    bool empty()
    {
        std::unique_lock<std::mutex> lock(mmutex);
        return mqueue.empty();
    }

    int32_t size()
    {
        std::unique_lock<std::mutex> lock(mmutex);
        return mqueue.size();
    }

    void enqueue(const T& t)
    {
        std::unique_lock<std::mutex> lock(mmutex);
        mqueue.emplace(t);
    }

    bool dequeue(T& t)
    {
        std::unique_lock<std::mutex> lock(mmutex);
        if(mqueue.empty())
        {
            return false;
        }
        t = std::move(mqueue.front());
        mqueue.pop();
        return true;
    }
private:
    std::queue<T> mqueue;
    std::mutex mmutex;

};