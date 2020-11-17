//
// Created by yuanzhihong on 2020/11/11.
//

#include "BoundedBlockingQueue.h"
#include <cassert>


template<typename T>
BoundedBlockingQueue<T>::BoundedBlockingQueue(int size)
    :mutex_(),notEmpty_(),notFull_(),queue_(),bound_(size)
{}

template<typename T>
void BoundedBlockingQueue<T>::put(const T &value) {
    std::lock_guard<std::mutex>lockGuard(mutex_);
    while (queue_.size() >= bound_){
        notFull_.wait(mutex_);
    }
    assert(!queue_.size() >= bound_);
    queue_.push_back(value);
    notEmpty_.notify_one();
}

template<typename T>
void BoundedBlockingQueue<T>::put(T &&value) {

}

template<typename T>
T BoundedBlockingQueue<T>::take() {
    std::lock_guard<std::mutex>lockGuard(mutex_);
    while (queue_.empty()){
        notEmpty_.wait(mutex_);
    }
    assert(!queue_.empty());
    T front(std::move(queue_.front()));
    queue_.pop_front();
    notFull_.notify_one();
    return front;
}

template<typename T>
bool BoundedBlockingQueue<T>::full()  {
    std::lock_guard<std::mutex>lockGuard(mutex_);
    return queue_.size() >= bound_;
}

template<typename T>
size_t BoundedBlockingQueue<T>::size()  {
    std::lock_guard<std::mutex>lockGuard(mutex_);
    return queue_.size();
}

template<typename T>
size_t BoundedBlockingQueue<T>::capacity()  {
    std::lock_guard<std::mutex>lockGuard(mutex_);
    return bound_ - queue_.size();
}
