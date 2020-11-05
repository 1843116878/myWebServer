//
// Created by 袁志红 on 2020/10/18.
//
#include "Thread.h"
#include "CurrentThread.h"
#include <assert.h>
#include <utility>
#include <zconf.h>
#include <syscall.h>
#include <sys/prctl.h>


namespace CurrentThread
{
    __thread int t_cachedTid = 0;
    __thread char t_tidString[32];
    __thread int t_tidStringLength = 6;
    __thread const char* t_threadName = "default";
    void cacheTid() {
        if (t_cachedTid == 0) {
            t_cachedTid = gettid();
            t_tidStringLength =
                    snprintf(t_tidString, sizeof t_tidString, "%5d ", t_cachedTid);
        }
    }
}
pid_t gettid(){
    return static_cast<pid_t>(::syscall(SYS_gettid));
};
//保存线程中的name和tid
struct ThreadData {
    using ThreadFunc = Thread::ThreadFunc;
    ThreadFunc func_;
    std::string name_;
    pid_t *tid_;
    CountDownLatch *latch_;

    ThreadData(const ThreadFunc &func, const std::string &name, pid_t *tid,
               CountDownLatch *latch)
            : func_(func), name_(name), tid_(tid), latch_(latch) {}

    void runInThread() {
        *tid_ = CurrentThread::tid();
        tid_ = nullptr;
        latch_->countDown();
        latch_ = nullptr;

        CurrentThread::t_threadName = name_.empty() ? "Thread" : name_.c_str();
        prctl(PR_SET_NAME, CurrentThread::t_threadName);

        func_();
        CurrentThread::t_threadName = "finished";
    }
};

void* startThread(void *obj) {
    ThreadData *data = static_cast<ThreadData *>(obj);
    data->runInThread();
    delete data;
    return nullptr;
}


int Thread::join() {
    assert(started_);
    assert(!joined_);
    if (thread_.joinable()) {
        joined_ = true;
        thread_.join();
    } else
        return -1;
    return 0;
}

void Thread::start() {
    assert(!started_);
    started_ = true;
    ThreadData *data = new ThreadData(func_, name_, &tid_, &latch_);
    thread_ = std::thread(startThread, data);
    started_ = false;
    delete data;
}

Thread::Thread(const ThreadFunc &func, std::string name)
        : thread_(), func_(func), name_(std::move(name)), id_(0), tid_(0), started_(false), joined_(false), latch_(1) {}

void Thread::setDefaultName() {
    if (name_.empty()){
        char buf[32];
        snprintf(buf, sizeof buf, "Thread");
    }
}

