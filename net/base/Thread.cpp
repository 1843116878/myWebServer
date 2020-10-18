//
// Created by 袁志红 on 2020/10/18.
//
#include "Thread.h"
#include <assert.h>
#include <utility>


namespace myWebServer::net::base::CurrentThread
{
    __thread int t_cachedTid = 0;
    __thread char t_tidString[32];
    __thread int t_tidStringLength = 6;
    __thread const char* t_threadName = "default";
}
namespace myWebServer::net::base
{
    int Thread::join() {
        assert(started_);
        assert(!joined_);
        if(thread_.joinable()) {
            joined_ = true;
            thread_.join();
        }
        else
            return -1;
        return 0;
    }
    void Thread::start() {
        assert(!started_);
        started_ = true;
        thread_ = std::thread(func_);
    }

    Thread::Thread(const ThreadFunc& func, std::string name)
    :thread_(),func_(func),name_(std::move(name)),tid_(0),pid_(0),started_(false),joined_(false),latch_(1)
    {}
}
