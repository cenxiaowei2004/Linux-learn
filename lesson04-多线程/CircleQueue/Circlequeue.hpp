#include <pthread.h>
#include <semaphore.h>
#include <iostream>
#include <vector>

using namespace std;

template <class T>
class CircleQueue {
private:
    void P(sem_t& sem) { sem_wait(&sem); }

    void V(sem_t& sem) { sem_post(&sem); }

public:
    CircleQueue(int _maxcap) : circlequeue(_maxcap), maxcap(_maxcap) {
        sem_init(&p_sem, 0, maxcap);
        sem_init(&c_sem, 0, 0);
        pthread_mutex_init(&pmutex, nullptr);
        pthread_mutex_init(&cmutex, nullptr);
        consumerstep = productorstep = 0;
    }

    void push(const T& in) {
        P(p_sem);
        pthread_mutex_lock(&pmutex);
        // success
        circlequeue[productorstep++] = in;
        productorstep %= maxcap;
        pthread_mutex_unlock(&pmutex);
        V(c_sem);
    }

    void pop(T* out) {
        P(c_sem);
        pthread_mutex_lock(&cmutex);
        *out = circlequeue[consumerstep++];
        consumerstep %= maxcap;
        pthread_mutex_unlock(&cmutex);
        V(p_sem);
    }

    ~CircleQueue() {
        sem_destroy(&p_sem);
        sem_destroy(&c_sem);
        pthread_mutex_destroy(&pmutex);
        pthread_mutex_destroy(&cmutex);
    }

private:
    vector<T> circlequeue;
    int maxcap;
    sem_t p_sem;  // 生产者看重空间资源
    sem_t c_sem;  // 消费者看重数据资源
    int consumerstep;
    int productorstep;
    pthread_mutex_t cmutex;
    pthread_mutex_t pmutex;
};