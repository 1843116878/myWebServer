//
// Created by yuanzhihong on 2020/10/22.
//

#ifndef MYWEBSERVER_LOGGING_H
#define MYWEBSERVER_LOGGING_H

#include <string>
#include "LogStream.h"

namespace myWebServer::net::base
{
    class AsyncLogging;

    class Logger
    {
    public:
        Logger(const char *fileName, int line);
        ~Logger();
        LogStream& stream(){
            return impl_.stream_;
        }

    private:
        class Impl      //内部类，防御编程
        {
        public:
            LogStream stream_;
            int line_;
            std::string basename_;
        };
        Impl impl_;
        static std::string logFileName_;
    };
}
#endif //MYWEBSERVER_LOGGING_H
