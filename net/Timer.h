//
// Created by yuanzhihong on 2020/10/31.
//

#ifndef MYWEBSERVER_TIMER_H
#define MYWEBSERVER_TIMER_H

#include <deque>
#include <memory>
#include <queue>
class HttpData;

class TimerNode{
public:
    TimerNode(std::shared_ptr<HttpData> requestData, int timeout);
    ~TimerNode();
    TimerNode(TimerNode &tn);
    void update(int timeout);
    bool isValid();
    void clearReq();
    void setDeleted();
    bool isDeleted() const;
    size_t getExpTime()const;

private:
    bool deleted_;
    size_t expiredTime_;
    std::shared_ptr<HttpData> SpHttpData;
};




#endif //MYWEBSERVER_TIMER_H
