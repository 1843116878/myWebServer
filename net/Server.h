// Created by yuanzhihong
//
#ifndef MYWEBSERVER_SERVER_H_
#define MYWEBSERVER_SERVER_H_

#include <memory>
#include "Channel.h"
#include "EventLoop.h"
#include "EventLoopThreadPool.h"

class Server {
public:
    Server(EventLoop *loop, int threadNum, int port);
    ~Server();

    void start();
    void handNewConn();
    void handThisConn();

    EventLoop *getLoop() const;
private:
    static const int MAXFDS = 100000;
    EventLoop *loop_;
    std::unique_ptr<EventLoopThreadPool> eventLoopThreadPool_;
    std::shared_ptr<Channel> acceptChannel_;
    int threadNum_;
    int port_;
    int listenFd_;
    bool started_;
};

#endif