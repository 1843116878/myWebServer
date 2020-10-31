//
// Created by yuanzhihong on 2020/10/23.
//

#include <cassert>
#include "AsyncLogging.h"

AsyncLogging::AsyncLogging(const std::string logFileName_, int flushInterval)
        :
        flushInterval_(flushInterval),
        running_(false),
        basename_(logFileName_),
        thread_(std::bind(&AsyncLogging::threadFunc, this), "Logging"),
        mutex_(),
        cond_(),
        latch_(1),
        currentBuffer_(new Buffer),
        nextBuffer_(new Buffer),
        buffers_() {
    assert(logFileName_.size() > 1);
    currentBuffer_->bzero();
    nextBuffer_->bzero();
    buffers_.reserve(16);
}

AsyncLogging::~AsyncLogging() {
    if (running_) stop();
}

void AsyncLogging::append(const char *logline, int len) {

}

void AsyncLogging::start() {
    running_ = true;
    thread_.start();
    latch_.wait();
}


void AsyncLogging::stop() {
    running_ = false;

}


