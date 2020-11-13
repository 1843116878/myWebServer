//
// Created by yuanzhihong on 2020/11/1.
//

#include "Epoll.h"
#include "Util.h"
#include "base/Logging.h"
#include <sys/epoll.h>
#include <cassert>
#include <queue>



const int EVENTNUM = 4096;
const int EPOLLWAIT_TIME = 10000;

Epoll::Epoll()
    :epollFd_(epoll_create1(EPOLL_CLOEXEC)), events_(EVENTNUM)
{
    assert(epollFd_ > 0);
}

Epoll::~Epoll()
{}

//注册一个新的描述符
void Epoll::epoll_add(SP_Channel request, int timeout) {
    int fd = request->getfd();
    if(timeout > 0){
        add_timer(request, timeout);
        fd2http_[fd] = request->getHolder();
    }
    epoll_event event;
    event.data.fd = fd;
    event.events = request->getEvents();

    request->EqualAndUpdateLastEvents();
    fd2chan_[fd] = request;
    if (epoll_ctl(epollFd_, EPOLL_CTL_ADD, fd, &event) < 0){
        perror("epoll_add error");
        fd2chan_[fd].reset();
    }
}

//移除一个描述符
void Epoll::epoll_del(SP_Channel request) {
    int fd = request->getfd();
    epoll_event event;
    event.data.fd = fd;
    event.events = request->getLastEvents();
    if(epoll_ctl(epollFd_, EPOLL_CTL_DEL, fd, &event) < 0){
        perror("epoll_del error");
    }
    fd2http_[fd].reset();
    fd2chan_[fd].reset();
}

//修改描述符状态
void Epoll::epoll_mod(SP_Channel request, int timeout) {
    if(timeout > 0)
        add_timer(request,timeout);
    int fd = request->getfd();
    if(!request->EqualAndUpdateLastEvents()){
        epoll_event event;
        event.data.fd = fd;
        event.events = request->getEvents();
        if(epoll_ctl(epollFd_, EPOLL_CTL_MOD, fd, &event) < 0){
            perror("epoll_mod error");
            fd2chan_[fd].reset();
        }
    }
}

//返回可以进行I/O事件的文件描述符集合
std::vector<SP_Channel> Epoll::poll() {
    while (true){
        int event_num =
                epoll_wait(epollFd_, &*events_.begin(),events_.size(),EPOLLWAIT_TIME);
        if (event_num < 0)
            perror("epoll wait error");
        std::vector<SP_Channel> req_data = getEventsRequest(event_num);
        if (req_data.size() > 0)
            return req_data;
    }
}


//事件分发的函数
std::vector<SP_Channel> Epoll::getEventsRequest(int events_num) {
    std::vector<SP_Channel> req_data;
    for (int i = 0; i < events_num; ++i){
        int fd = events_[i].data.fd;
        SP_Channel  cur_req = fd2chan_[fd];
        if(cur_req){
            cur_req->setRevents(events_[i].events);
            cur_req->setEvents(0);
            //加入线程池钱将Timer和request分离
            req_data.push_back(cur_req);
        }
        else
            LOG << "SP cur_req is invalid";
    }
    return req_data;
}

void Epoll::add_timer(SP_Channel request_data, int timeout) {
    std::shared_ptr<HttpData> t = request_data->getHolder();
    if(t)
        timerManager_.addTimer(t, timeout);
    else
        LOG << "timer add fail";
}

int Epoll::getEpollFd() {
    return epollFd_;
}

void Epoll::handleExpired() {
    timerManager_.handleExpiredEvent();
}

