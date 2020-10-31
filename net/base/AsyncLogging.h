//
// Created by yuanzhihong on 2020/10/23.
//

#ifndef MYWEBSERVER_ASYNCLOGGING_H
#define MYWEBSERVER_ASYNCLOGGING_H
#include <functional>
#include <mutex>
#include <condition_variable>
#include <vector>
#include <string>
#include "Thread.h"
#include "LogStream.h"
#include "CountDownLatch.h"


class AsyncLogging {
public:
    AsyncLogging(const AsyncLogging &) = delete;

    const AsyncLogging &operator=(const AsyncLogging &) = delete;

public:
    AsyncLogging(const std::string basename, int flushInterval = 2);

    ~AsyncLogging();

    void append(const char *logline, int len);

    void start();

    void stop();

public:
    using Buffer = FixedBuffer<KLargeBuffer>;
    using BufferVector = std::vector<std::shared_ptr<Buffer>>;
    using BufferPtr = std::shared_ptr<Buffer>;
private:
    void threadFunc();

    bool running_;
    Thread thread_;
    std::string basename_;
    const int flushInterval_;
    std::mutex mutex_;
    std::condition_variable_any cond_;
    CountDownLatch latch_;
    BufferPtr currentBuffer_;
    BufferPtr nextBuffer_;
    BufferVector buffers_;
};


#endif //MYWEBSERVER_ASYNCLOGGING_H
