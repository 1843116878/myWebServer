//
// Created by yuanzhihong on 2020/11/4.
//

#ifndef MYWEBSERVER_SERVER_H
#define MYWEBSERVER_SERVER_H
#include <memory>
#include "Channel.h"
#include "EventLoop.h"
#include "EventLoopThreadPool.h"

class Server{
public:
    Server(EventLoop *loop, int threadNum, int port);
    ~Server();
    EventLoop *getLoop() const;
    void start();
    void handNewConn();
    void handThisConn();
private:
    EventLoop *loop_;
    int threadNum_;
    std::unique_ptr<EventLoopThreadPool> eventLoopThreadPool_;
    bool started_;
    std::shared_ptr<Channel> acceptChannel_;
    int port_;
    int listenFd_;
    static const int MAXFDS = 100000;
};
#endif //MYWEBSERVER_SERVER_H
