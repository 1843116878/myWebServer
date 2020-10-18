//
// Created by 袁志红 on 2020/10/18.
//
#include "CountDownLatch.h"
namespace myWebServer::net::base
{
    CountDownLatch::CountDownLatch(int count)
    :mutex_(),condition_(),count_()
    {}
    void CountDownLatch::countDown() {
        std::lock_guard<std::recursive_mutex>lockGuard(mutex_);
        --count_;
        condition_.notify_all();
    }
    void CountDownLatch::wait() {
        std::lock_guard<std::recursive_mutex> lockGuard(mutex_);
        condition_.wait(mutex_,[](int count){ return count == 0;}(count_));
    }
}

