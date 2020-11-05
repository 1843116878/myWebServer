//
// Created by yuanzhihong on 2020/11/4.
//

#ifndef MYWEBSERVER_EVENTLOOPTHREAD_H
#define MYWEBSERVER_EVENTLOOPTHREAD_H

#include <mutex>
#include <condition_variable>
#include "EventLoop.h"
#include "base/Thread.h"


class EventLoopThread {
public:
    EventLoopThread(const EventLoopThread&) = delete;
    EventLoopThread& operator=(const EventLoopThread&) = delete;
public:
    EventLoopThread();
    ~EventLoopThread();
    EventLoop *startLoop();
private:
    void threadFunc();
    EventLoop *loop_;
    bool exiting_;
    Thread thread_;
    std::mutex mutex_;
    std::condition_variable_any cond_;
};


#endif //MYWEBSERVER_EVENTLOOPTHREAD_H
