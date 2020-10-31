//
// Created by yuanzhihong on 2020/10/31.
//

#include "Timer.h"

TimerNode::TimerNode(std::shared_ptr<HttpData> requestData, int timeout) {

}

TimerNode::~TimerNode() {

}

TimerNode::TimerNode(TimerNode &tn) {

}

void TimerNode::update(int timeout) {

}

bool TimerNode::isValid() {
    return false;
}

void TimerNode::clearReq() {

}

void TimerNode::setDeleted() {

}

bool TimerNode::isDeleted() const {
    return false;
}

size_t TimerNode::getExpTime() const {
    return 0;
}



