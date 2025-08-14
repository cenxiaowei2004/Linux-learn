#include <threads.h>
#include <iostream>
#include <queue>
#include <vector>
#include "Mutex.hpp"
#include "Thread.hpp"

using namespace std;

template <class T>
class ThreadPool {
public:
    ThreadPool(int _num = 10) : num(_num) {
        pthread_mutex_init(&mutex, nullptr);
        pthread_cond_init(&cond, nullptr);
        for (int i = 0; i < num; i++) {
            threads.push_back(new Thread(handlerTask, this, i));
        }
    }

    static void* handlerTask(void* args) {
        ThreadPool<T>* threadpool = static_cast<ThreadPool<T>*>(args);
        while (true) {
            threadpool->lockQueue();
            while (threadpool->isEmpty()) {
                threadpool->waitQueue();
            }
            T task = threadpool->pop();
            threadpool->unlockQueue();
            cout << "deal with task..." << endl;
            task();
            cout << pthread_self() << endl;
        }
        delete threadpool;
        return nullptr;
    }

    void lockQueue() { pthread_mutex_lock(&mutex); }
    void unlockQueue() { pthread_mutex_unlock(&mutex); }
    bool isEmpty() { return taskQueue.empty(); }
    void waitQueue() { pthread_cond_wait(&cond, &mutex); }
    T pop() {
        T task = taskQueue.front();
        taskQueue.pop();
        return task;
    }

    void run() {
        for (const auto& thread : threads) {
            thread->start();
            cout << "thread:" << thread->getname() << endl;
        }
    }

    void push(const T& task) {
        pthread_mutex_lock(&mutex);
        taskQueue.push(task);
        pthread_cond_signal(&cond);
        pthread_mutex_unlock(&mutex);
    }

    ~ThreadPool() {
        pthread_mutex_destroy(&mutex);
        pthread_cond_destroy(&cond);
        for (const auto& thread : threads) {
            delete thread;
        }
    }

private:
    int num;
    vector<Thread*> threads;
    queue<T> taskQueue;
    pthread_mutex_t mutex;
    pthread_cond_t cond;
};