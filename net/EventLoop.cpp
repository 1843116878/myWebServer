//
// Created by yuanzhihong on 2020/11/1.
//

#include <sys/epoll.h>
#include <sys/eventfd.h>
#include <unistd.h>
#include <cassert>
#include "Util.h"
#include "base/Logging.h"
#include "EventLoop.h"


__thread EventLoop *t_loopInThisThread = 0;
int createEventfd(){
    int evtfd = eventfd(0, EFD_NONBLOCK | EFD_CLOEXEC);
    if(evtfd < 0){
        LOG<<"Fail in eventfd";
        abort();
    }
    return evtfd;
}
EventLoop::EventLoop()
    :looping_(false),
    poller_(std::make_shared<Epoll>()),
    wakeupFd_(createEventfd()),
    pwakeupChannel_(std::make_shared<Channel>(this, wakeupFd_)),
    quit_(false),
    eventHandling_(false),
    callingPendingFunctors_(false),
    threadId_(CurrentThread::tid()),
{
    if(t_loopInThisThread){
         LOG << "Another EventLoop " << t_loopInThisThread <<
         " exists in this thread " << threadId_;
    } else{
        t_loopInThisThread = this;
    }
    pwakeupChannel_->setEvents(EPOLLIN | EPOLLET);
    pwakeupChannel_->setReadHandler(std::bind(&EventLoop::handleRead, this));
    pwakeupChannel_->setConnHandler(std::bind(&EventLoop::handleConn, this));
    poller_->epoll_add(pwakeupChannel_, 0);
}

EventLoop::~EventLoop() {
    close(wakeupFd_);
    t_loopInThisThread = nullptr;
}

void EventLoop::loop() {
    assert(!looping_);
    assert(isInLoopThread());
    looping_ = true;
    quit_ = false;
    std::vector<SP_Channel> res;
    while (!quit_){
        res.clear();
        res = poller_->poll();
        eventHandling_ = true;
        for(auto &it:res)
            it->handleEvents();
        eventHandling_ = false;
        doPendingFunctors();
        poller_->handleExpired();
    }
    looping_ = false;
}

void EventLoop::quit() {
    quit_ = true;
    if(!isInLoopThread())
        wakeup();
}

void EventLoop::runInLoop(EventLoop::Functor &&cb) {

}

void EventLoop::queueInLoop(EventLoop::Functor &&cb) {

}

bool EventLoop::isInLoopThread() const {
    return false;
}

void EventLoop::assertInLoopThread() {

}

void EventLoop::shutdown(std::shared_ptr<Channel> channel) {

}

void EventLoop::removeFromPoller(std::shared_ptr<Channel> channel) {

}

void EventLoop::updatePoller(std::shared_ptr<Channel> channel, int timeout) {

}

void EventLoop::addToPoller(std::shared_ptr<Channel> channel, int timeout) {

}

void EventLoop::wakeup() {

}

void EventLoop::handleRead() {

}

void EventLoop::doPendingFunctors() {

}

void EventLoop::handleConn() {

}

