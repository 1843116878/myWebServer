//
// Created by yuanzhihong on 2020/10/23.
//

#include <cassert>
#include <memory>
#include <chrono>
#include "AsyncLogging.h"
#include "LogFile.h"

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
    std::lock_guard<std::mutex> lockGuard(mutex_);
    if(currentBuffer_->avail() > len)
        currentBuffer_->append(logline, len);
    else{
        buffers_.push_back(currentBuffer_);
        currentBuffer_->reset();
        if(nextBuffer_)
            currentBuffer_=std::move(nextBuffer_);
        else
            currentBuffer_.reset(new Buffer);
        currentBuffer_->append(logline, len);
        cond_.notify_one();
    }
}

void AsyncLogging::threadFunc() {
    assert(running_ == true);
    latch_.countDown();
    LogFile output(basename_);
    BufferPtr  newBuffer1 = std::make_unique<Buffer>();
    BufferPtr  newBuffer2 = std::make_unique<Buffer>();
    newBuffer1->bzero();
    newBuffer2->bzero();
    BufferVector buffersToWrite;
    buffersToWrite.reserve(16);
    while (running_){
        assert(newBuffer1 && newBuffer1->length() == 0);
        assert(newBuffer2 && newBuffer2->length() == 0);
        assert(buffersToWrite.empty());

        {
            std::lock_guard<std::mutex> lockGuard(mutex_);
            if(buffers_.empty())
                cond_.wait(mutex_, std::chrono::seconds(flushInterval_));
            buffers_.push_back(currentBuffer_);
            currentBuffer_.reset();
            currentBuffer_ = std::move(newBuffer1);
            buffersToWrite.swap(buffers_);
            if(!nextBuffer_)
                nextBuffer_ = std::move(newBuffer2);
        }

        assert(!buffersToWrite.empty());

        if(buffersToWrite.size() > 25){
            buffersToWrite.erase(buffersToWrite.begin() + 2,buffersToWrite.end());
        }

        if(!newBuffer1){
            assert(!buffersToWrite.empty());
            newBuffer1 = buffersToWrite.back();
            buffersToWrite.pop_back();
            newBuffer1->reset();
        }

        if(!newBuffer2){
            assert(!buffersToWrite.empty());
            newBuffer2 = buffersToWrite.back();
            buffersToWrite.pop_back();
            newBuffer2->reset();
        }

        buffersToWrite.clear();
        output.flush();
    }
    output.flush();
}
void AsyncLogging::start() {
    running_ = true;
    thread_.start();
    latch_.wait();
}


void AsyncLogging::stop() {
    running_ = false;
    cond_.notify_one();
    thread_.join();
}


