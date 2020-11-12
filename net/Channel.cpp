//
// Created by yuanzhihong on 2020/11/1.
//

#include "Channel.h"

Channel::Channel(EventLoop *loop)
    :loop_(loop), fd_(0), events_(0), lastEvents(0)
{}

Channel::Channel(EventLoop *loop, int fd)
    :loop_(loop), fd_(fd), events_(0),lastEvents(0)
{}

Channel::~Channel()
{}

int Channel::getfd() {
    return fd_;
}

void Channel::setfd(int fd) {
    fd_ = fd;
}

std::shared_ptr<HttpData> Channel::getHolder() {
    std::shared_ptr<HttpData> res(holder_.lock());
    return res;
}

void Channel::setHolder(const std::shared_ptr<HttpData>& holder) {
    holder_ = holder;
}

void Channel::setRevents(__uint32_t ev) {
    revents_ = ev;
}

void Channel::setEvents(__uint32_t ev) {
    events_ = ev;
}

__uint32_t &Channel::getEvents() {
    return events_;
}

__uint32_t Channel::getLastEvents() {
    return lastEvents;
}

void Channel::handleRead() {
    if(readHandler_)
        readHandler_();
}

void Channel::handleWrite() {
    if(writeHandler_)
        writeHandler_();
}

void Channel::handleError(int fd, int err_num, std::string short_msg)
{}

void Channel::handleConn() {
    if(connHandler_)
        connHandler_();
}

void Channel::setReadHandler(Channel::CallBack &&readHandler) {
    readHandler_ = readHandler;
}

void Channel::setWriteHandler(Channel::CallBack &&writeHandler) {
    writeHandler_ = writeHandler;
}

void Channel::setErrorHandler(Channel::CallBack &&errorHandler) {
    errorHandler_ = errorHandler;
}

void Channel::setConnHandler(Channel::CallBack &&connHandler) {
    connHandler_ = connHandler;
}

bool Channel::EqualAndUpdateLastEvents() {
    bool res = (lastEvents == events_);
    lastEvents = events_;
    return res;
}

void Channel::handleEvents() {
    events_ = 0;
    if ((revents_ & EPOLLHUP) && !(revents_ &EPOLLIN)){
        events_ = 0;
        return;
    }
    if (revents_ & EPOLLERR){
        if(errorHandler_)
            errorHandler_();
        events_ = 0;
        return;
    }
    if (revents_ & (EPOLLIN | EPOLLPRI | EPOLLRDHUP))
        handleRead();
    if(revents_ & EPOLLOUT)
        handleWrite();
    handleConn();
}

