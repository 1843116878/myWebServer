//
// Created by yuanzhihong on 2020/10/31.
//

#include <sys/time.h>
#include "Timer.h"

TimerNode::TimerNode(std::shared_ptr<HttpData> requestData, int timeout)
    :deleted_(false), SPHttpData(requestData)
{
    timeval now;
    gettimeofday(&now, nullptr);
    //以毫秒记
    expiredTime_ = (((now.tv_sec % 10000)*10000) + (now.tv_usec/1000)) + timeout;
}

TimerNode::~TimerNode() {
    if(SPHttpData)
        SPHttpData->handleClose();
}

TimerNode::TimerNode(TimerNode &tn)
    :SPHttpData(tn.SPHttpData), expiredTime_(0),deleted_(false)
{}

void TimerNode::update(int timeout) {
    timeval now;
    gettimeofday(&now, nullptr);
    expiredTime_ = (((now.tv_sec % 10000)*10000) + (now.tv_usec/1000)) + timeout;
}

bool TimerNode::isValid() {
    timeval now;
    gettimeofday(&now, nullptr);
    size_t temp = ((now.tv_sec % 10000)*10000) + (now.tv_usec/1000);
    if(temp < expiredTime_)
        return true;
    else{
        this->setDeleted();
        return false;
    }
}

void TimerNode::clearReq() {
    SPHttpData.reset();
    this->setDeleted();
}

void TimerNode::setDeleted() {
    deleted_ = true;
}

bool TimerNode::isDeleted() const {
    return deleted_;
}

size_t TimerNode::getExpTime() const {
    return expiredTime_;
}


TimerManager::TimerManager()
= default;

TimerManager::~TimerManager()
= default;

void TimerManager::addTimer(std::shared_ptr<HttpData> SPHttpData, int timeout) {
    SPTimerNode newNode = std::make_shared<TimerNode>(SPHttpData, timeout);
    timerNodeQueue.push(newNode);
    SPHttpData->linkTimer(newNode);
}

/* 处理逻辑是这样的~
 *因为优先队列不支持随机访问
 *即使支持，随机删除某节点后破坏了堆的结构，需要重新更新堆结构。
 *所以对于被置为deleted的时间节点，会延迟到它超时 或
 *它前面的节点都被删除时，它才会被删除。
 *一个点被置为deleted,它最迟会在TIMER_TIME_OUT时间后被删除。
 *这样做有两个好处：
 *第一个好处是不需要遍历优先队列，省时。
 *第二个好处是给超时时间一个容忍的时间，就是设定的超时时间是删除的下限(并不是一到超时时间就立即删除)，如果监听的请求在超时后的下一次请求中又一次出现了，
 *就不用再重新申请RequestData节点了，这样可以继续重复利用前面的RequestData，减少了一次delete和一次new的时间。
*/
void TimerManager::handleExpiredEvent() {
    while (!timerNodeQueue.empty()){
        SPTimerNode ptimeNow = timerNodeQueue.top();
        if(ptimeNow->isDeleted())
            timerNodeQueue.pop();
        else if(!ptimeNow->isValid())
            timerNodeQueue.pop();
        else
            break;
    }
}
