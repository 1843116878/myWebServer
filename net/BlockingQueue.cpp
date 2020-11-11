//
// Created by yuanzhihong on 2020/11/11.
//

#include "BlockingQueue.h"
#include <cassert>
template<typename T>
BlockingQueue<T>::BlockingQueue()
    :mutex_(), notEmpty(), queue_()
{}

template<typename T>
void BlockingQueue<T>::put(const T &value) {
    std::lock_guard<std::mutex> lockGuard(mutex_);
    queue_.push_back(value);
    notEmpty.notify_one();
}

template<typename T>
void BlockingQueue<T>::put(T &&value) {
    std::lock_guard<std::mutex>lockGuard(mutex_);
    queue_.push_back(std::move(value));
    notEmpty.notify_one();
}

template<typename T>
T BlockingQueue<T>::take() {
    std::lock_guard<std::mutex> lockGuard(mutex_);
    while (queue_.empty()){
        notEmpty.wait(mutex_);
    }
    assert(!queue_.empty());
    T front(std::move(queue_.front()));
    queue_.pop_front();
    return front;
}

template<typename T>
size_t BlockingQueue<T>::size()  {
    std::lock_guard<std::mutex> lockGuard(mutex_);
    return queue_.size();
}
