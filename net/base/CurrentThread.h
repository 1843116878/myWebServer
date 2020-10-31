//
// Created by 袁志红 on 2020/10/18.
//

#ifndef MYWEBSERVER_CURRENTTHREAD_H
#define MYWEBSERVER_CURRENTTHREAD_H

namespace CurrentThread
{
    extern __thread int t_cachedTid;
    extern __thread char t_tidString[32];
    extern __thread int t_tidStringLength;
    extern __thread const char* t_threadName;
    void cacheTid();
    inline int tid() {
        //__builtin_expect的意思是预判，条件式大概率式后面的结果，方便编译器优化
        if (__builtin_expect(t_cachedTid == 0, 0)) {
            cacheTid();
        }
        return t_cachedTid;
    }

    inline const char* tidString()  // for logging
    {
        return t_tidString;
    }

    inline int tidStringLength()  // for logging
    {
        return t_tidStringLength;
    }

    inline const char* name() { return t_threadName; }
}
}
#endif //MYWEBSERVER_CURRENTTHREAD_H
