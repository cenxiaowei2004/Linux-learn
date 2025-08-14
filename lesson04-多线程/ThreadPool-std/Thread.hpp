#pragma once

#include <pthread.h>
#include <cassert>
#include <cstring>
#include <functional>
#include <iostream>
#include <string>

namespace ThreadNs {
typedef std::function<void*(void*)> func_t;
const int num = 1024;

class Thread {
private:
    // 在类内创建线程，想让线程执行对应的方法，需要将方法设置成为static
    static void* start_routine(void* args)  // 类内成员，有缺省参数！
    {
        Thread* _this = static_cast<Thread*>(args);
        return _this->callback();
    }

public:
    Thread() {
        char namebuffer[num];
        snprintf(namebuffer, sizeof namebuffer, "thread-%d", threadnum++);
        name_ = namebuffer;
    }

    void start(func_t func, void* args = nullptr) {
        func_ = func;
        args_ = args;
        int n = pthread_create(&tid_, nullptr, start_routine, this);  // TODO
        assert(n == 0);
        (void)n;
    }

    void join() {
        int n = pthread_join(tid_, nullptr);
        assert(n == 0);
        (void)n;
    }

    std::string threadname() { return name_; }

    ~Thread() {
        // do nothing
    }
    void* callback() { return func_(args_); }

private:
    std::string name_;
    func_t func_;
    void* args_;
    pthread_t tid_;

    static int threadnum;
};
int Thread::threadnum = 1;
}  // end namespace ThreadNs