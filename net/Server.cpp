//
// Created by yuanzhihong on 2020/11/4.
//

#include <netinet/in.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include "Server.h"

Server::Server(EventLoop *loop, int threadNum, int port)
    :loop_(loop),
    threadNum_(threadNum),
    eventLoopThreadPool_(std::make_unique<EventLoopThreadPool>(loop_, threadNum)),
    port_(port),
    listenFd_(socket_bind_listen(port_))
{
    acceptChannel_->setfd(listenFd_);
    handle_for_sigpipe();
    if(setSocketNonBlocking(listenFd_) < 0){
        perror("set socket non block failed");
        abort();
    }
}

Server::~Server()
{}

EventLoop *Server::getLoop() const {
    return loop_;
}

void Server::start() {
    eventLoopThreadPool_->start();
    acceptChannel_->setEvents(EPOLLIN | EPOLLET);
    acceptChannel_->setReadHandler([this] { handNewConn(); });
    acceptChannel_->setConnHandler([this] { handThisConn(); });
    loop_->addToPoller(acceptChannel_, 0);
    started_ = true;
}

void Server::handNewConn() {
    sockaddr_in client_addr;
    memset(&client_addr, 0, sizeof(client_addr));
    socklen_t client_addr_len = sizeof(client_addr);
    int accept_fd = 0;
    while ((accept_fd = accept(listenFd_, (sockaddr *)&client_addr, &client_addr_len)) > 0){
        EventLoop *loop = eventLoopThreadPool_->getNextLoop();
        LOG << " New connection from "<< inet_ntoa(client_addr.sin_addr) << ":"
            << ntohs(client_addr.sin_port);
        if(accept_fd >= MAXFDS){
            close(accept_fd);
            continue;
        }
        if(setSocketNonBlocking(accept_fd) < 0){
            LOG << " Set non block failed! ";
            return;
        }
        setSocketNodelay(accept_fd);
        std::shared_ptr<HttpData> req_info = std::make_shared<HttpData>(loop, accept_fd);
        req_info->getChannel()->setHolder(req_info);
        loop->queueInLoop([req_info] { req_info->newEvent(); });
    }
    acceptChannel_->setEvents(EPOLLIN | EPOLLET);
}

void Server::handThisConn() {
    loop_->updatePoller(acceptChannel_);
}

