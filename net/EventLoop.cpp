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
    pwakeupChannel_->setReadHandler([this] { handleRead(); });
    pwakeupChannel_->setConnHandler([this] { handleConn(); });
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
    if(isInLoopThread())
        cb();
    else
        queueInLoop(std::move(cb));
}

void EventLoop::queueInLoop(EventLoop::Functor &&cb) {
    {
        std::lock_guard<std::mutex> lockGuard(mutex_);
        pendingFunctors.emplace_back(std::move(cb));
    }
    if(!isInLoopThread() || callingPendingFunctors_)
        wakeup();
}

bool EventLoop::isInLoopThread() const {
    return threadId_ == CurrentThread::tid();
}

void EventLoop::assertInLoopThread() {
    assert(isInLoopThread());
}

void EventLoop::shutdown(std::shared_ptr<Channel> channel) {
    shutDownWR(channel->getfd());
}

void EventLoop::removeFromPoller(std::shared_ptr<Channel> channel) {
    poller_->epoll_del(channel);
}

void EventLoop::updatePoller(std::shared_ptr<Channel> channel, int timeout) {
    poller_->epoll_mod(channel, timeout);
}

void EventLoop::addToPoller(std::shared_ptr<Channel> channel, int timeout) {
    poller_->epoll_add(channel, timeout);
}

void EventLoop::wakeup() {
    uint64_t one = 1;
    ssize_t n = writen(wakeupFd_,(char *)(&one), sizeof one);
    if(n != sizeof one){
        LOG << "EventLoop::wakeup() writes " << n << " bytes instead of 8";
    }
}

void EventLoop::handleRead() {
    uint64_t one = 1;
    ssize_t n = readn(wakeupFd_, &one, sizeof one);
    if(n != sizeof one){
        LOG << " EventLoop::handleRead() reads " << n << " bytes instead of 8 ";
    }
    pwakeupChannel_->setEvents(EPOLLIN | EPOLLET);
}

void EventLoop::doPendingFunctors() {
    std::vector<Functor> functors;
    callingPendingFunctors_ = true;
    {
        std::lock_guard<std::mutex> lockGuard(mutex_);
        functors.swap(pendingFunctors);
    }
    for (auto & functor : functors){
        functor();
    }
    callingPendingFunctors_ = false;

}

void EventLoop::handleConn() {
    updatePoller(pwakeupChannel_, 0);
}

