// Created by yuanzhihong
//
#ifndef MYWEBSERVER_UTIL_H_
#define MYWEBSERVER_UTIL_H_

#include <unistd.h>
#include <deque>
#include <memory>
#include <queue>
#include <mutex>
#include "HttpData.h"

class HttpData;

class TimerNode {
public:
    TimerNode(TimerNode &tn);
    TimerNode(std::shared_ptr<HttpData> requestData, int timeout);
    ~TimerNode();

    void update(int timeout);
    void clearReq();

    size_t getExpTime() const;

    void setDeleted();
    bool isDeleted() const;
    bool isValid();

private:
    std::shared_ptr<HttpData> SPHttpData;
    size_t expiredTime_;
    bool deleted_;
};

struct TimerCmp {
    bool operator()(std::shared_ptr<TimerNode> &a, std::shared_ptr<TimerNode> &b) const {
        return a->getExpTime() > b->getExpTime();
    }
};

class TimerManager {
public:
    TimerManager();
    ~TimerManager();
    void addTimer(std::shared_ptr<HttpData> SPHttpData, int timeout);
    void handleExpiredEvent();
private:
    using SPTimerNode = std::shared_ptr<TimerNode>;
    std::priority_queue<SPTimerNode, std::deque<SPTimerNode>, TimerCmp> timerNodeQueue;
    std::mutex mutex_;
};

#endif