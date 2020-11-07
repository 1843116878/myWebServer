//
// Created by yuanzhihong on 2020/11/4.
//

#ifndef MYWEBSERVER_EVENTLOOPTHREADPOOL_H
#define MYWEBSERVER_EVENTLOOPTHREADPOOL_H
#include <memory>
#include <vector>
#include "EventLoopThread.h"
#include "base/Logging.h"


class EventLoopThreadPool{
public:
    EventLoopThreadPool(const EventLoopThreadPool&) = delete;
    const EventLoopThreadPool& operator= (const EventLoopThreadPool&) = delete;
public:
    EventLoopThreadPool(EventLoop *baseLoop, int threadNums);
    ~EventLoopThreadPool();
    void start();
    EventLoop *getNextLoop();
private:
    EventLoop *baseLoop_;
    bool started_;
    int threadNums_;
    int next_;
    std::vector<std::shared_ptr<EventLoopThread>>threads_;
    std::vector<EventLoop*> loops_;

};


#endif //MYWEBSERVER_EVENTLOOPTHREADPOOL_H
