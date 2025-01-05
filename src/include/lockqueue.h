#pragma once

#include<queue>
#include<thread>
#include<mutex>
#include<condition_variable>

template <typename T>
class LockQueue
{
public:
    void Push(const T& data)
    {
        std::lock_guard<std::mutex> guard(_mutx);
        _queue.push(data);
        _full.notify_one();
    }
    T Pop()
    {
        std::unique_lock<std::mutex> lock(_mutx);
        _full.wait(lock,[&](){
            return !_queue.empty();
        });
        T data=_queue.front();
        _queue.pop();
        return data;
    }
private:
    std::queue<T> _queue;
    std::mutex _mutx;
    std::condition_variable _full;
};