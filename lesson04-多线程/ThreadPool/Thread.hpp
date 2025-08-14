#include <pthread.h>
#include <cassert>
#include <functional>
#include <iostream>

using namespace std;

class Thread {
public:
    using func_t = function<void*(void*)>;
    const int num = 1024;
    Thread(func_t _func, void* _args, int number) : func(_func), args(_args) {
        char buffer[1024];
        snprintf(buffer, sizeof buffer, "thread-%d", number);
        name = buffer;
    }

    static void* start_routine(void* args) {
        Thread* thread = static_cast<Thread*>(args);
        return thread->callback();
    }

    void join() {
        int n = pthread_join(pit, nullptr);
        assert(n == 0);
        (void)n;
    }

    void start() {
        int n = pthread_create(&pit, nullptr, start_routine, this);
        assert(n == 0);
        (void)n;
    }

    string getname() { return name; }

    void* callback() { return func(args); }

    ~Thread() { /* do nothing */ }

private:
    string name;
    pthread_t pit;
    func_t func;
    void* args;
};