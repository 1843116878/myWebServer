//
// Created by yuanzhihong on 2020/11/4.
//
#include <functional>
#include <cassert>
#include <mutex>
#include "EventLoopThread.h"

void EventLoopThread::threadFunc() {
    EventLoop loop;
    {
        std::lock_guard<std::mutex> lockGuard(mutex_);
        loop_ = &loop;
        cond_.notify_one();
    }
    loop.loop();
    loop_ = nullptr;
}

EventLoop *EventLoopThread::startLoop() {
    assert(!thread_.started());
    EventLoop loop;
    {
        std::lock_guard<std::mutex> lockGuard(mutex_);
        while (loop_ == nullptr)
            cond_.wait(mutex_);//一直等到threadFun在Thread中运行
    }
    return loop_;
}

EventLoopThread::~EventLoopThread() {
    assert(!thread_.started());
    if(loop_ != nullptr){
        loop_->quit();
        thread_.join();
    }
}

EventLoopThread::EventLoopThread()
    :loop_(nullptr),
    exiting_(false),
    thread_([this] { threadFunc(); },"EventLoopThread"),
    mutex_(),
    cond_()
{}
