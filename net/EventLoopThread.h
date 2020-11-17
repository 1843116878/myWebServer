// Created by yuanzhihong
//
#ifndef MYWEBSERVER_EVENTLOOPTHREAD_H_
#define MYWEBSERVER_EVENTLOOPTHREAD_H_

#include "EventLoop.h"
#include "base/Thread.h"
class EventLoopThread {
public:
    EventLoopThread(const EventLoopThread &) = delete;
    const EventLoopThread &operator=(const EventLoopThread &) = delete;
public:
    EventLoopThread();
    ~EventLoopThread();
    EventLoop *startLoop();
private:
    void threadFunc();

    EventLoop *loop_;
    Thread thread_;
    bool exiting_;
    std::mutex mutex_;
    std::condition_variable_any cond_;
};

#endif
