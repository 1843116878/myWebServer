//
// Created by yuanzhihong on 2020/10/22.
//

#include "LogFile.h"
#include <cassert>
#include <memory>

LogFile::LogFile(const std::string &basename, int flushEveryN)
    :basename_(basename),
    flushEveryN_(flushEveryN),
    count_(0),
    mutex_(){
    file_ = std::make_unique<AppendFile>(basename);
}

LogFile::~LogFile() {}

void LogFile::append(const char *logline, int len) {
    std::lock_guard<std::mutex> lockGuard(mutex_);
    append_unlocked(logline, len);
}

void LogFile::flush() {
    std::lock_guard<std::mutex> lockGuard(mutex_);
    file_->fluse();
}


void LogFile::append_unlocked(const char *logline, int len) {
    file_->append(logline, len);
    ++count_;
    if(count_ >= flushEveryN_){
        count_ = 0;
        file_->fluse();
    }
}

