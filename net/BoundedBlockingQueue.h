//
// Created by yuanzhihong on 2020/11/11.
//

#ifndef MYWEBSERVER_BOUNDEDBLOCKINGQUEUE_H
#define MYWEBSERVER_BOUNDEDBLOCKINGQUEUE_H

#include <mutex>
#include <condition_variable>
#include <queue>
#include <array>

template<typename T>
class BoundedBlockingQueue {
public:
    BoundedBlockingQueue(const  BoundedBlockingQueue&) = delete;
    const BoundedBlockingQueue& operator= (const  BoundedBlockingQueue&) = delete;

public:
    BoundedBlockingQueue(int size);
    void put(const T&value);
    void put(T &&value);
    T take();
    bool full();
    size_t size();
    size_t capacity();
private:
    std::mutex mutex_;
    std::condition_variable_any notEmpty_;
    std::condition_variable_any notFull_;
    std::deque<T> queue_;
    size_t bound_;
};




#endif //MYWEBSERVER_BOUNDEDBLOCKINGQUEUE_H
