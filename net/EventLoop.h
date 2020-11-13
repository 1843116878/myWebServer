//
// Created by yuanzhihong on 2020/11/1.
//

#ifndef MYWEBSERVER_EVENTLOOP_H
#define MYWEBSERVER_EVENTLOOP_H

#include <functional>
#include <memory>
#include <vector>
#include <iostream>
#include "base/CurrentThread.h"
#include "base/Logging.h"
#include "base/Thread.h"
#include "Util.h"
#include "Epoll.h"
#include "Channel.h"



class EventLoop{
public:
    using Functor = std::function<void()>;
    EventLoop();
    ~EventLoop();
    void loop();
    void quit();
    void runInLoop(Functor &&cb);
    void queueInLoop(Functor &&cb);
    bool isInLoopThread() const;
    void assertInLoopThread();
    void shutdown(std::shared_ptr<Channel> channel);
    void removeFromPoller(std::shared_ptr<Channel> channel);
    void updatePoller(std::shared_ptr<Channel> channel, int timeout = 0);
    void addToPoller(std::shared_ptr<Channel> channel, int timeout = 0);
private:
    void wakeup();
    void handleRead();
    void doPendingFunctors();
    void handleConn();

    bool looping_;
    int wakeupFd_;
    bool quit_;
    bool eventHandling_;
    bool callingPendingFunctors_;
    std::shared_ptr<Epoll> poller_;
    std::shared_ptr<Channel> pwakeupChannel_;
    mutable std::mutex mutex_;
    std::vector<Functor> pendingFunctors;
    const pid_t threadId_;
};
#endif //MYWEBSERVER_EVENTLOOP_H
