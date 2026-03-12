#pragma once

#include <condition_variable>
#include <mutex>
#include <queue>
#include <vector>

// 通过 condition_variable + mutex 实现生产者消费者模型的线程池

#define MAX_NUM 5

template <class T>
class threadpool {
private:
    // 静态工作线程函数
    static void* Routine(void* arg) {
        threadpool* pool = (threadpool*)arg;
        while (pool->running_) {
            T task;
            // 等待，外部 Push 之后唤醒：
            pool->Pop(&task);
            printf("thread pool receive a new task\n");
            task();
        }
        return nullptr;
    }

public:
    threadpool(int thread_num = MAX_NUM) : thread_num_(thread_num), running_(true) {
        pthread_mutex_init(&mutex_, nullptr);
        pthread_cond_init(&push_cond_, nullptr);
        pthread_cond_init(&pop_cond_, nullptr);
    }

    void threadPoolInit() {
        pthread_t tid;
        for (int i = 0; i < thread_num_; i++) {
            pthread_create(&tid, nullptr, Routine, this);
            threads_.push_back(tid);
        }
        printf("thread pool status: OK\n");
    }

    void Lock() { pthread_mutex_lock(&mutex_); }

    void Unlock() { pthread_mutex_unlock(&mutex_); }

    bool isEmpty() { return thread_queue_.empty(); }

    bool isFull() { return thread_queue_.size() == MAX_NUM; }

    void Push(const T& task) {
        Lock();
        // condition_variable 使用 while
        while (running_ && isFull()) {                // 队列满时等待
            pthread_cond_wait(&push_cond_, &mutex_);  // 需要另一个条件变量
        }

        thread_queue_.push(task);
        Unlock();
        pthread_cond_signal(&pop_cond_);
    }

    void Pop(T* task) {
        Lock();
        // condition_variable 使用 谓词
        while (running_ && isEmpty()) {
            pthread_cond_wait(&pop_cond_, &mutex_);
        }
        *task = thread_queue_.front();
        thread_queue_.pop();
        Unlock();
        pthread_cond_signal(&push_cond_);
    }

    ~threadpool() {
        pthread_mutex_lock(&mutex_);
        running_ = false;
        pthread_mutex_unlock(&mutex_);

        pthread_cond_broadcast(&push_cond_);
        pthread_cond_broadcast(&pop_cond_);

        for (pthread_t& p : threads_) {
            pthread_join(p, nullptr);
        }

        pthread_mutex_destroy(&mutex_);
        pthread_cond_destroy(&push_cond_);
        pthread_cond_destroy(&pop_cond_);
        running_ = false;
    }

private:
    bool running_;
    std::queue<T> thread_queue_;
    int thread_num_;
    pthread_mutex_t mutex_;
    pthread_cond_t push_cond_;
    pthread_cond_t pop_cond_;
    std::vector<pthread_t> threads_;
};