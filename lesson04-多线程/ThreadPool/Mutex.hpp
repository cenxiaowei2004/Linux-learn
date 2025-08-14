#include <iostream>
#include <unistd.h>
#include <pthread.h>

using namespace std;

class Mutex {
public:
    Mutex(pthread_mutex_t* lock_p_ = nullptr) : lock_p(lock_p_) {}
    ~Mutex() {}
    void lock() {
        if(lock_p) pthread_mutex_lock(lock_p);
    }
    void unlock() {
        if(lock_p) pthread_mutex_unlock(lock_p);
    }
private:
    pthread_mutex_t* lock_p;
};


class LockGuard {
public:
    LockGuard(pthread_mutex_t* lock_p_) : mutex(lock_p_) {
        mutex.lock();
    }
    ~LockGuard() {
        mutex.unlock();
    }

private:
    Mutex mutex;
};