//
// Created by 袁志红 on 2020/10/18.
//
#include <mutex>
#include <condition_variable>
#ifndef MYWEBSERVER_COUNTDOWNLATCH_H
#define MYWEBSERVER_COUNTDOWNLATCH_H
namespace myWebServer::net::base
{
    class CountDownLatch{
    public:
        CountDownLatch(const CountDownLatch&) = delete;
        const CountDownLatch& operator= (const CountDownLatch&)= delete;
    public:
        explicit CountDownLatch(int count);
        void wait();
        void countDown();
    private:
        std::recursive_mutex mutex_;
        std::condition_variable_any condition_;
        int count_;
    };
}
#endif //MYWEBSERVER_COUNTDOWNLATCH_H
