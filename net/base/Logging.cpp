//
// Created by yuanzhihong on 2020/10/22.
//

#include "Logging.h"
#include "Thread.h"
#include "CurrentThread.h"
#include "AsyncLogging.h"
#include <iostream>
#include <ctime>
#include <sys/time.h>

namespace myWebServer::net::base
{
    static AsyncLogging *AsyncLogger_;

    std::string Logger::logFileName_ = "./WebServer.log";

    void once_init()
    {
        AsyncLogger_ = new AsyncLogging(Logger::getLogFileName());

    }
    void output(const char *msg, int len)
    {

    }

    Logger::Impl::Impl(const char *filename, int line)
    :stream_(), line_(line), basename_(filename)
    {
        formatTime();
    }
    void Logger::Impl::formatTime()
    {
        struct timeval tv;
        time_t  time;
        char str_t[26] = {0};
        gettimeofday(&tv, NULL);
        time = tv.tv_sec;
        struct tm *p_time = localtime(&time);
        strftime(str_t, 26, "%Y-%m-%d %H:%M:%S\n", p_time);
        stream_ << str_t;
    }
    Logger::Logger(const char *fileName, int line)
    : impl_(fileName, line)
    {}
    Logger::~Logger()
    {
        impl_.stream_<<" -- " <<impl_.basename_<<':'<<impl_.line_<<'\n';
        const LogStream::Buffer& buf(stream().buffer());
        output(buf.data(), buf.length());
    }

}
