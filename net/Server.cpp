// Created by yuanzhihong
//
#include "Server.h"
#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include "Util.h"
#include "base/Logging.h"


Server::Server(EventLoop *loop, int threadNum, int port)
    : loop_(loop),
      eventLoopThreadPool_(std::make_unique<EventLoopThreadPool>(loop_,threadNum)),
      acceptChannel_(std::make_shared<Channel>(loop_)),
      threadNum_(threadNum),
      port_(port),
      listenFd_(socket_bind_listen(port_)),
      started_(false)
{
    acceptChannel_->setFd(listenFd_);
    handle_for_sigpipe();
    if (setSocketNonBlocking(listenFd_) < 0) {
        perror("set socket non block failed");
        abort();
    }
}

Server::~Server() = default;

void Server::start() {
    eventLoopThreadPool_->start();
    acceptChannel_->setEvents(EPOLLIN | EPOLLET);
    acceptChannel_->setReadHandler([this] { handNewConn(); });
    acceptChannel_->setConnHandler([this] { handThisConn(); });
    loop_->addToPoller(acceptChannel_, 0);
    started_ = true;
}

void Server::handNewConn() {
    struct sockaddr_in client_addr{};
    memset(&client_addr, 0, sizeof(struct sockaddr_in));
    socklen_t client_addr_len = sizeof(client_addr);
    int accept_fd = 0;
    while ((accept_fd = accept(listenFd_, (struct sockaddr *) &client_addr, &client_addr_len)) > 0) {
        EventLoop *loop = eventLoopThreadPool_->getNextLoop();
        LOG << "New connection from " << inet_ntoa(client_addr.sin_addr) << ":" << ntohs(client_addr.sin_port);

        if (accept_fd >= MAXFDS) {
            close(accept_fd);
            continue;
        }
        // 设为非阻塞模式
        if (setSocketNonBlocking(accept_fd) < 0) {
            LOG << "Set non block failed!";
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

EventLoop *Server::getLoop() const {
    return loop_;
}
