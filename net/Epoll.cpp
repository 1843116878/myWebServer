//
// Created by yuanzhihong on 2020/11/1.
//

#include "Epoll.h"

Epoll::Epoll() {

}

Epoll::~Epoll() {

}

void Epoll::epoll_add(SP_Channel request, int timeout) {

}

void Epoll::epoll_del(SP_Channel request) {

}

void Epoll::epoll_mod(SP_Channel request, int timeout) {

}

std::vector<SP_Channel> Epoll::poll() {
    return std::vector<SP_Channel>();
}

std::vector<SP_Channel> Epoll::getEventsRequest(int events_num) {
    return std::vector<SP_Channel>();
}

void Epoll::add_timer(SP_Channel request_data, int timeout) {

}

int Epoll::getEpollFd() {
    return epollFd_;
}

void Epoll::handleExpired() {

}

