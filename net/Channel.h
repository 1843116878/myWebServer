//
// Created by yuanzhihong on 2020/11/1.
//

#ifndef MYWEBSERVER_CHANNEL_H
#define MYWEBSERVER_CHANNEL_H

#include <sys/epoll.h>
#include <functional>
#include <unordered_map>
#include <memory>
#include <string>
#include "Timer.h"

class EventLoop;
class HttpData;

class Channel{
public:
    using CallBack = std::function<void()>;
    Channel(EventLoop *loop);
    Channel(EventLoop *loop, int fd);
    ~Channel();

    int getfd();
    void setfd(int fd);

    std::shared_ptr<HttpData> getHolder();
    void setHolder(const std::shared_ptr<HttpData>& holder);

    void setRevents(__uint32_t ev);
    void setEvents(__uint32_t ev);
    __uint32_t &getEvents();
    __uint32_t getLastEvents();


    void handleRead();
    void handleWrite();
    void handleError(int fd, int err_num, std::string short_msg);
    void handleConn();

    void setReadHandler(CallBack &&readHandler);
    void setWriteHandler(CallBack &&writeHandler);
    void setErrorHandler(CallBack &&errorHandler);
    void setConnHandler(CallBack &&connHandler);

    bool EqualAndUpdateLastEvents();
    //事件处理函数
    void handleEvents();
private:
    CallBack readHandler_;
    CallBack writeHandler_;
    CallBack errorHandler_;
    CallBack connHandler_;
private:
    EventLoop *loop_;
    int fd_;
    __uint32_t events_;
    __uint32_t revents_;
    __uint32_t lastEvents;
    // 方便找到上层持有该Channel的对象
    std::weak_ptr<HttpData> holder_;
};
using SP_Channel = std::shared_ptr<Channel>;

#endif //MYWEBSERVER_CHANNEL_H
