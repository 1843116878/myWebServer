// Created by yuanzhihong
//
#include "Channel.h"
#include <unistd.h>
#include "EventLoop.h"


Channel::Channel(EventLoop *loop)
        : loop_(loop), events_(0), lastEvents_(0), fd_(0) {}

Channel::Channel(EventLoop *loop, int fd)
        : loop_(loop), fd_(fd), events_(0), lastEvents_(0) {}

Channel::~Channel() {}

int Channel::getFd() { return fd_; }

void Channel::setFd(int fd) { fd_ = fd; }

void Channel::handleRead() {
    if (readHandler_) {
        readHandler_();
    }
}

void Channel::handleWrite() {
    if (writeHandler_) {
        writeHandler_();
    }
}

void Channel::handleConn() {
  if (connHandler_) {
    connHandler_();
  }
}

void Channel::setHolder(std::shared_ptr<HttpData> holder) {
    holder_ = holder;
}

std::shared_ptr<HttpData> Channel::getHolder() {
    std::shared_ptr<HttpData> res = holder_.lock();
    return res;
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

void Channel::handleEvents() {
    events_ = 0;
    if ((revents_ & EPOLLHUP) && !(revents_ & EPOLLIN)) {
        events_ = 0;
        return;
    }
    if (revents_ & EPOLLERR) {
        if (errorHandler_) errorHandler_();
        events_ = 0;
        return;
    }
    if (revents_ & (EPOLLIN | EPOLLPRI | EPOLLRDHUP)) {
        handleRead();
    }
    if (revents_ & EPOLLOUT) {
        handleWrite();
    }
    handleConn();
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

bool Channel::EqualAndUpdateLastEvents() {
    bool ret = (lastEvents_ == events_);
    lastEvents_ = events_;
    return ret;
}

__uint32_t Channel::getLastEvents() {
    return lastEvents_;
}

