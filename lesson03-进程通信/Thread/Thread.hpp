#include <iostream>
#include <pthread.h>
#include <functional>
#include <cassert>


using namespace std;

class Thread;

class Context {
public:
    Thread* th;
    void* args;
    Context() : th(nullptr), args(nullptr) {}
    ~Context() {}
};

class Thread {
public:
    using func_t = function<void*(void*)>;
    const int num = 1024;
    Thread(func_t _func, void* _args, int number) : func(_func), args(_args) {
        char buffer[1024];
        snprintf(buffer, sizeof buffer, "thread-%d", number);
        name = buffer;
        Context* ctx = new Context();
        ctx->th = this;
        ctx->args = this->args;
        int n = pthread_create(&pit, nullptr, start_routine, ctx);
        assert(n == 0);
    }
    
    static void* start_routine(void* args) {
        Context* ctx = static_cast<Context*>(args);
        void* ret = ctx->th->run(ctx->args);
        delete ctx;
        return ret;
    }

    void join() {
        int n = pthread_join(pit, nullptr);
        assert(n == 0);
    }

    void* run(void* args) {
        return func(args);
    }

    ~Thread() { /* do nothing */ }
private: 
    string name;
    pthread_t pit;
    func_t func;
    void* args;
};