// Created by yuanzhihong
//
#include "EventLoopThread.h"

EventLoopThread::EventLoopThread()
    : loop_(nullptr),
      thread_([this] { threadFunc(); }, "EventLoopThread"),
      exiting_(false),
      mutex_(),
      cond_() {}

EventLoopThread::~EventLoopThread() {
    exiting_ = true;
    if (loop_ != nullptr) {
        loop_->quit();
        thread_.join();
    }
}

EventLoop* EventLoopThread::startLoop() {
    assert(!thread_.started());
    thread_.start();
    {
        std::lock_guard<std::mutex> lock(mutex_);
        // 一直等到threadFun在Thread里真正跑起来
        while (loop_ == nullptr)
            cond_.wait(mutex_);
    }
    return loop_;
}

void EventLoopThread::threadFunc() {
    EventLoop loop;
    {
        std::lock_guard<std::mutex> lock(mutex_);
        loop_ = &loop;
        cond_.notify_one();
    }
    loop.loop();
    // assert(exiting_);
    loop_ = nullptr;
}