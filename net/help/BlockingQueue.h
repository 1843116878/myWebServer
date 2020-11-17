//
// Created by yuanzhihong on 2020/11/11.
//

#ifndef MYWEBSERVER_BLOCKINGQUEUE_H
#define MYWEBSERVER_BLOCKINGQUEUE_H

#include <mutex>
#include <condition_variable>
#include <queue>

template<typename T>
class BlockingQueue {
public:
    BlockingQueue(const BlockingQueue&) = delete;
    const BlockingQueue& operator= (const BlockingQueue&) = delete;

public:
    BlockingQueue();
    void put(const  T&);
    void put(T &&);
    T take();
    size_t size() ;
private:
    std::mutex mutex_;
    std::condition_variable_any notEmpty;
    std::deque<T>   queue_;
};




#endif //MYWEBSERVER_BLOCKINGQUEUE_H
