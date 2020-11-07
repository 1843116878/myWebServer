//
// Created by yuanzhihong on 2020/11/4.
//

#include <cassert>
#include "EventLoopThreadPool.h"


EventLoopThreadPool::EventLoopThreadPool(EventLoop *baseLoop, int threadNums)
    :baseLoop_(baseLoop), started_(false), threadNums_(threadNums), next_(0)
{
    if(threadNums <= 0) {
        LOG << "threadNums <= 0";
        abort();
    }
}

EventLoopThreadPool::~EventLoopThreadPool() {
    LOG << "EventLoopThreadPoll() delete ";
}

void EventLoopThreadPool::start() {
    baseLoop_->assertInLoopThread();
    started_ = true;
    for (int i = 0; i < threadNums_; ++i) {
        std::shared_ptr<EventLoopThread> t = std::make_shared<EventLoopThread>();
        threads_.push_back(t);
        loops_.push_back(t->startLoop());
    }
}

EventLoop *EventLoopThreadPool::getNextLoop() {
    baseLoop_->assertInLoopThread();
    assert(started_);
    EventLoop *loop = baseLoop_;
    if(!loops_.empty()){
        loop = loops_[next_];
        next_ = (next_ + 1) % threadNums_;
    }
    return loop;
}

