//
// Created by 袁志红 on 2020/10/18.
//

#ifndef MYWEBSERVER_THREAD_H
#define MYWEBSERVER_THREAD_H

#include <functional>
#include <thread>
#include "CountDownLatch.h"
namespace myWebServer::net::base
{
    class Thread
    {
    public:
        using ThreadFunc = std::function<void ()>;
    public:
        Thread(const ThreadFunc&ThreadFunc, std::string name = "");
        Thread(const Thread&) = delete;

        Thread& operator= (const Thread&)=delete;
    public:
        void start();
        int join();
    public:
        bool started(){ return started_;};
        bool joined(){ return joined_;};
        pid_t pid(){ return tid_;};
        std::string name()const { return name_;};
    private:
        std::thread thread_;
        pid_t tid_;                 //进程id
        std::thread::id id_;       //线程id
        ThreadFunc func_;           //线程要执行的任务
        bool started_;              //用来标记是否执行过
        bool joined_;               //用来标记是否join过
        std::string name_;
        CountDownLatch latch_;
    };
}
#endif //MYWEBSERVER_THREAD_H
