/* This folder was created by cen on 2025/05/27 */
#include <iostream>
#include <queue>
#include <pthread.h>

using namespace std;

template<class T>
class BlockQueue {
public:
    BlockQueue(const int& maxcap_ = 5) : maxcap(maxcap_) {
        pthread_mutex_init(&mutex, nullptr);
        pthread_cond_init(&p_cond, nullptr);
        pthread_cond_init(&c_cond, nullptr);
    }
    ~BlockQueue() {
        pthread_mutex_destroy(&mutex);
        pthread_cond_destroy(&p_cond);
        pthread_cond_destroy(&c_cond);
    }

    /**
     * tips:
     * 输入型参数：const &
     * 输出型参数：*
     * 输入输出型参数：&
     */
    void push(const T& in) {
        // 生产者
        // 加锁：保证资源的安全
        pthread_mutex_lock(&mutex);
        // 满了的情况下：
        while(q.size() == maxcap) {
            // 生产者进行等待...
            pthread_cond_wait(&p_cond, &mutex); 
            // pthread_cond_wait:会先将锁释放，之后挂起
        }
        q.push(in);
        // 唤醒消费者
        pthread_cond_signal(&c_cond);
        pthread_mutex_unlock(&mutex);
    }
    void pop(T* out) {
        // 消费者
        // 加锁：保证资源的安全
        pthread_mutex_lock(&mutex);
        // 空了的情况下：
        while(q.empty()) {
            // 消费者进行等待...
            pthread_cond_wait(&c_cond, &mutex); 
        }
        *out = q.front();
        q.pop();
        // 唤醒生产者
        pthread_cond_signal(&p_cond);
        pthread_mutex_unlock(&mutex);
    }


private:
    queue<T> q;
    int maxcap;
    pthread_mutex_t mutex;
    pthread_cond_t p_cond;  // 生产者对应的条件变量
    pthread_cond_t c_cond;  // 消费者对应的条件变量
};