#include <pthread.h>
#include <iostream>
#include <queue>

using namespace std;
/**
 * tip:
 * 输入型参数：condt &
 * 输出型参数：*
 * 输入输出型参数：&
 */
template <class T>
class BlockQueue {
public:
    BlockQueue(int _maxcap) : maxcap(_maxcap) {
        pthread_mutex_init(&mutex, nullptr);
        pthread_cond_init(&p_cond, nullptr);
        pthread_cond_init(&c_cond, nullptr);
    }

    void push(const T& in) {
        // productor pushes data
        pthread_mutex_lock(&mutex);
        while (isFull()) {
            // full?productor waiting...
            pthread_cond_wait(&p_cond, &mutex);
        }
        q.push(in);
        // wakeup consumer
        pthread_cond_signal(&c_cond);
        pthread_mutex_unlock(&mutex);
    }

    void pop(T* out) {
        // consumer pops data
        pthread_mutex_lock(&mutex);
        if (isEmpty()) {
            pthread_cond_wait(&c_cond, &mutex);
        }
        *out = q.front();
        q.pop();
        pthread_cond_signal(&p_cond);
        pthread_mutex_unlock(&mutex);
    }

    ~BlockQueue() {
        pthread_mutex_destroy(&mutex);
        pthread_cond_destroy(&p_cond);
        pthread_cond_destroy(&c_cond);
    }

private:
    bool isFull() { return q.size() == maxcap; }
    bool isEmpty() { return q.empty(); }

private:
    queue<T> q;
    int maxcap;
    pthread_mutex_t mutex;
    pthread_cond_t p_cond;
    pthread_cond_t c_cond;
};