// Created by yuanzhihong
//
#ifndef MYWEBSERVER_HTTPDATA_H_
#define MYWEBSERVER_HTTPDATA_H_
#include <sys/epoll.h>
#include <unistd.h>
#include <functional>
#include <map>
#include <memory>
#include <string>
#include <unordered_map>
#include "Timer.h"


class EventLoop;
class TimerNode;
class Channel;

enum class ProcessState {
    STATE_PARSE_URI = 1,
    STATE_PARSE_HEADERS,
    STATE_RECV_BODY,
    STATE_ANALYSIS,
    STATE_FINISH
};

enum class URIState {
    PARSE_URI_AGAIN = 1,
    PARSE_URI_ERROR,
    PARSE_URI_SUCCESS,
};

enum class HeaderState {
    PARSE_HEADER_SUCCESS = 1,
    PARSE_HEADER_AGAIN,
    PARSE_HEADER_ERROR
};

enum class AnalysisState {
    ANALYSIS_SUCCESS = 1,
    ANALYSIS_ERROR
};

enum class ParseState {
    H_START = 0,
    H_KEY,
    H_COLON,
    H_SPACES_AFTER_COLON,
    H_VALUE,
    H_CR,
    H_LF,
    H_END_CR,
    H_END_LF
};

enum class ConnectionState {
    H_CONNECTED = 0,
    H_DISCONNECTING,
    H_DISCONNECTED
};

enum class HttpMethod {
    METHOD_POST = 1,
    METHOD_GET,
    METHOD_HEAD
};

enum class HttpVersion {
    HTTP_10 = 1,
    HTTP_11
};

//这是基于pthread_once的单例模式，见muduo或者陈硕大佬的博客
class MimeType {
private:
    static void init();
    MimeType();
    MimeType(const MimeType &m);
public:
    static std::string getMime(const std::string &suffix);
private:
    static pthread_once_t once_control;
    static std::unordered_map<std::string, std::string> mime;
};


class HttpData : public std::enable_shared_from_this<HttpData> {
public:
    HttpData(EventLoop *loop, int connfd);
    ~HttpData();
    void reset();

    void handleClose();
    void newEvent();

    void seperateTimer();
    void linkTimer(std::shared_ptr<TimerNode> mtimer);

    std::shared_ptr<Channel> getChannel();
    EventLoop *getLoop();

private:
    void handleRead();
    void handleWrite();
    void handleConn();
    void handleError(int fd, int err_num, std::string short_msg);

    URIState parseURI();
    HeaderState parseHeaders();
    AnalysisState analysisRequest();
private:
    EventLoop *loop_;
    std::shared_ptr<Channel> channel_;
    std::map<std::string, std::string> headers_;
    std::weak_ptr<TimerNode> timer_;
    int fd_;
    std::string fileName_;
    std::string path_;

    //输入和输出缓冲区,使用的string
    std::string inBuffer_;
    std::string outBuffer_;
    int nowReadPos_;

    bool error_;
    bool keepAlive_;

    HttpMethod method_;
    HttpVersion HTTPVersion_;
    ProcessState state_;
    ParseState hState_;
    ConnectionState connectionState_;
};

#endif