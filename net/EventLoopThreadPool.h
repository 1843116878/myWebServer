// Created by yuanzhihong
//
#pragma once
#include <memory>
#include <vector>
#include "EventLoopThread.h"
#include "base/Logging.h"



class EventLoopThreadPool  {
public:
    EventLoopThreadPool(const EventLoopThreadPool&)=delete;
    const EventLoopThreadPool& operator=(const EventLoopThreadPool&)=delete;
public:
    EventLoopThreadPool(EventLoop *baseLoop, int numThreads);
    ~EventLoopThreadPool();

    void start();
    EventLoop *getNextLoop();

private:
    EventLoop *baseLoop_;
    bool started_;
    int numThreads_;
    int next_;
    std::vector<std::shared_ptr<EventLoopThread>> threads_;
    std::vector<EventLoop *> loops_;
};