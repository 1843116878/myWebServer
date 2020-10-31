//
// Created by yuanzhihong on 2020/10/21.
//

#include "Util.h"

#include <cerrno>
#include <csignal>
#include <cstring>
#include <unistd.h>
#include <netinet/in.h>
#include <netinet/tcp.h>
#include <fcntl.h>
#include <sys/socket.h>


const int MAX_BUFF = 4096;
ssize_t readn(int fd, void *buff, size_t n) {
    size_t readSum = 0, nread = 0, nleft = n;
    char *ptr = (char *)buff;
    while(nleft > 0){
        if((nread = read(fd, ptr, nleft)) < 0){
            if(errno == EINTR)
                nread = 0;
            else if(errno == EAGAIN)
                return readSum;
            else
                return -1;
        }
        else if(nread == 0)
            break;
        readSum += nread;
        nleft -= nread;
        ptr += nread;
    }
    return readSum;
}

ssize_t readn(int fd, std::string &inBuffer, bool &zero) {
    ssize_t nread = 0, readSum = 0;
    while (true){
        char buff[MAX_BUFF];
        if((nread = read(fd, buff, MAX_BUFF)) < 0) {
            if (errno == EINTR)
                continue;
            else if (errno == EAGAIN)
                return readSum;
            else {
                perror("read error");
                return -1;
            }
        }
        else if(nread == 0){
            zero = true;
            break;
        }
        readSum += nread;
        inBuffer += std::string (buff, buff + nread);
    }
    return readSum;
}

ssize_t readn(int fd, std::string &inBuffer) {
    size_t nread = 0, readSum = 0;
    while (true){
        char buff[MAX_BUFF];
        if((nread = read(fd, buff, MAX_BUFF)) < 0){
            if(errno == EINTR)
                continue;
            else if(errno == EAGAIN)
                return readSum;
            else{
                perror("read error");
                return -1;
            }
        }
        else if(nread == 0)
            break;
        readSum += nread;
        inBuffer += std::string(buff, buff + nread);
    }
    return readSum;
}

ssize_t writen(int fd, void *buff, size_t n) {
    size_t nleft = n, nwritten = 0, writeSum = 0;
    char *ptr =(char *)buff;
    while (nleft > 0){
        if((nwritten = write(fd, ptr, nleft)) <= 0){
            if(nwritten < 0){
                if(errno == EINTR){
                    nwritten = 0;
                    continue;;
                }
                else if(errno == EAGAIN)
                    return writeSum;
                else
                    return -1;
            }
        }
        writeSum += nwritten;
        nleft -= nwritten;
        ptr += nwritten;
    }
    return writeSum;
}

ssize_t writen(int fd, std::string &sbuff) {
    size_t nleft = sbuff.size(), nwritten  = 0;
    size_t writeSum = 0;
    const char *ptr = sbuff.c_str();
    while (nleft > 0){
        if((nwritten = write(fd, ptr, nleft)) <= 0){
            if(nwritten < 0){
                if(errno == EINTR){
                    nwritten = 0;
                    continue;
                }
                else if(errno == EAGAIN)
                    break;
                else
                    return -1;
            }
        }
        writeSum += nwritten;
        nleft -= nwritten;
        ptr += nwritten;
    }
    if(writeSum == static_cast<int>(sbuff.size()))
        sbuff.clear();
    else
        sbuff = sbuff.substr(writeSum);
    return writeSum;
}

void handle_for_sigpipe() {
    struct sigaction sa;
    memset(&sa, '\0', sizeof(sa));
    sa.sa_handler = SIG_IGN;
    sa.sa_flags = 0;
    if(sigaction(SIGPIPE, &sa, NULL))
        return;
}

int setSocketReuseAdd(int fd){
    int opt = 1;
    if(setsockopt(fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) == -1){
        close(fd);
        return -1;
    }
    return 0;
}
int setSocketNonBlocking(int fd) {
    int flag;
    if((flag = fcntl(fd, F_GETFL, 0)) == -1)
        return -1;
    flag |= O_NONBLOCK;
    if(fcntl(fd, F_SETFD, flag) == -1)
        return -1;
    return 0;
}

void setSocketNodelay(int fd) {
    int enable = 1;
    setsockopt(fd, IPPROTO_TCP, TCP_NODELAY, (void *)&enable, sizeof(enable));
}

void setSocketNoLinger(int fd) {
    linger linger_;
    linger_.l_onoff = 1;
    linger_.l_linger = 30;
    setsockopt(fd, SOL_SOCKET, SO_LINGER, (const char *)&linger_, sizeof(linger_));
}

void shutDownWR(int fd) {
    shutdown(fd, SHUT_WR);
}

int socket_bind_listen(int port) {
    if(port < 0 || port >65535) return -1;
    int listen_fd = 0;
    sockaddr_in server_addr{};
    if((listen_fd = socket(AF_INET, SOCK_STREAM, 0)) == -1)
        return -1;
    if(setSocketReuseAdd(listen_fd) == -1)
        return -1;
    bzero((char *)&server_addr, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = port;
    server_addr.sin_addr.s_addr = htonl(INADDR_ANY);
    if(bind(listen_fd, (sockaddr *)&server_addr, sizeof(server_addr)) == -1){
        close(listen_fd);
        return -1;
    }
    if(listen(listen_fd, 2048) == -1){
        close(listen_fd);
        return -1;
    }
    if(listen_fd == -1){
        close(listen_fd);
        return -1;
    }
    return listen_fd;
}

