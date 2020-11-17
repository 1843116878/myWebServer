// Created by yuanzhihong
//
#ifndef MYWEBSERVER_EVENTLOOP_H_
#define MYWEBSERVER_EVENTLOOP_H_


#include <functional>
#include <memory>
#include <vector>
#include <cassert>
#include <mutex>
#include "Channel.h"
#include "Epoll.h"
#include "Util.h"
#include "base/CurrentThread.h"
#include "base/Logging.h"
#include "base/Thread.h"


#include <iostream>


class EventLoop {
 public:
    using Functor = std::function<void()>;

    EventLoop();
    ~EventLoop();

    void loop();
    void quit();

    void runInLoop(Functor &&cb);
    void queueInLoop(Functor &&cb);

    bool isInLoopThread() const;
    void assertInLoopThread() const;
    void shutdown(std::shared_ptr<Channel> channel);

    void removeFromPoller(std::shared_ptr<Channel> channel);
    void updatePoller(std::shared_ptr<Channel> channel, int timeout = 0);
    void addToPoller(std::shared_ptr<Channel> channel, int timeout = 0);
private:
    void wakeup();
    void handleRead();
    void doPendingFunctors();
    void handleConn();

private:
    // 声明顺序 wakeupFd_ > pwakeupChannel_
    int wakeupFd_;
    bool looping_;
    bool quit_;
    bool eventHandling_;
    bool callingPendingFunctors_;
    std::shared_ptr<Epoll> poller_;
    std::shared_ptr<Channel> pwakeupChannel_;
    std::vector<Functor> pendingFunctors_;

    const pid_t threadId_;
    mutable std::mutex mutex_;
};

#endif
