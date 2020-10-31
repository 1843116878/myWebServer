//
// Created by yuanzhihong on 2020/10/22.
//

#ifndef MYWEBSERVER_LOGFILE_H
#define MYWEBSERVER_LOGFILE_H

#include <mutex>
#include <memory>
#include <string>
#include "FileUtil.h"

class LogFile{
public:
    LogFile(const LogFile&) = delete;
    const LogFile& operator=(const LogFile&)=delete;
    // 每被append flushEveryN次，flush一下，会往文件写，只不过，文件也是带缓冲区的
    LogFile(const std::string &basename, int flushEveryN = 1024);
    ~LogFile();

public:
    void append(const char *logline, int len);
    void flush();
private:
    void append_unlocked(const char *logline, int len);

    const std::string basename_;
    const int flushEveryN_;
    int count_;
    std::mutex mutex_;
    std::unique_ptr<AppendFile> file_;
};
#endif //MYWEBSERVER_LOGFILE_H
