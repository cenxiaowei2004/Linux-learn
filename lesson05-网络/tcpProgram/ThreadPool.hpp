#pragma once

#include <pthread.h>
#include <unistd.h>
#include <mutex>
#include <queue>
#include <vector>
#include "LockGuard.hpp"
#include "Thread.hpp"

using namespace ThreadNs;

const int gnum = 3;

template <class T>
class ThreadPool;

template <class T>
class ThreadData {
public:
    ThreadPool<T>* threadpool;
    std::string name;

public:
    ThreadData(ThreadPool<T>* tp, const std::string& n) : threadpool(tp), name(n) {}
};

template <class T>
class ThreadPool {
private:
    static void* handlerTask(void* args) {
        ThreadData<T>* td = (ThreadData<T>*)args;
        while (true) {
            T t;
            {
                LockGuard lockguard(td->threadpool->mutex());
                while (td->threadpool->isQueueEmpty()) {
                    td->threadpool->threadWait();
                }
                t = td->threadpool->pop();
            }
            std::cout << td->name << " 获取了一个任务： " << std::endl;
            t();
        }
        delete td;
        return nullptr;
    }

    ThreadPool(const int& num = gnum) : _num(num) {
        pthread_mutex_init(&_mutex, nullptr);
        pthread_cond_init(&_cond, nullptr);
        for (int i = 0; i < _num; i++) {
            _threads.push_back(new Thread());
        }
    }

    // 饿汉单例模式：赋值运算符重载和拷贝构造函数删除
    void operator=(const ThreadPool&) = delete;
    ThreadPool(const ThreadPool&) = delete;

public:
    void lockQueue() { pthread_mutex_lock(&_mutex); }
    void unlockQueue() { pthread_mutex_unlock(&_mutex); }
    bool isQueueEmpty() { return _task_queue.empty(); }
    void threadWait() { pthread_cond_wait(&_cond, &_mutex); }
    T pop() {
        T t = _task_queue.front();
        _task_queue.pop();
        return t;
    }
    pthread_mutex_t* mutex() { return &_mutex; }

public:
    void run() {
        logMessage(NORMAL, "threadPool init success");
        for (const auto& t : _threads) {
            ThreadData<T>* td = new ThreadData<T>(this, t->threadname());
            t->start(handlerTask, td);
            std::cout << t->threadname() << " start ..." << std::endl;
        }
    }
    void push(const T& in) {
        LockGuard lockguard(&_mutex);
        _task_queue.push(in);
        pthread_cond_signal(&_cond);
    }
    ~ThreadPool() {
        pthread_mutex_destroy(&_mutex);
        pthread_cond_destroy(&_cond);
        for (const auto& t : _threads)
            delete t;
    }

    // 获取单例对象
    static ThreadPool<T>* getInstance() {
        if (tp == nullptr) {
            _singlock.lock();
            if (nullptr == tp) {
                tp = new ThreadPool<T>();
            }
            _singlock.unlock();
        }
        return tp;
    }

private:
    int _num;
    std::vector<Thread*> _threads;
    std::queue<T> _task_queue;
    pthread_mutex_t _mutex;
    pthread_cond_t _cond;
    // 静态单例指针
    static ThreadPool<T>* tp;
    static std::mutex _singlock;
};

template <class T>
// 初始化为 nullptr
ThreadPool<T>* ThreadPool<T>::tp = nullptr;

template <class T>
std::mutex ThreadPool<T>::_singlock;
