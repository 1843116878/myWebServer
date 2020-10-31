//
// Created by yuanzhihong on 2020/10/22.
//

#ifndef MYWEBSERVER_LOGGING_H
#define MYWEBSERVER_LOGGING_H

#include <string>
#include "LogStream.h"
#include "AsyncLogging.h"

class AsyncLogging;

#define LOG Logger(__FILE__, __LINE__).stream()

class Logger {
public:
    Logger(const char *fileName, int line);

    ~Logger();

    LogStream &stream() {
        return impl_.stream_;
    }

    static void setLogFileName(std::string filename) {
        logFileName_ = filename;
    }

    static std::string getLogFileName() {
        return logFileName_;
    }

private:
    class Impl      //内部类，防御编程
    {
        void formatTime();

    public:
        LogStream stream_;
        int line_;
        std::string basename_;

        Impl(const char *filename, int line);
    };

    Impl impl_;
    static std::string logFileName_;
};

#endif //MYWEBSERVER_LOGGING_H
