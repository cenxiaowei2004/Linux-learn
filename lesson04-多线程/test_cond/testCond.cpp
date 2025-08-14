#include <pthread.h>
#include <unistd.h>
#include <iostream>

using namespace std;

// 定义条件变量和互斥锁
pthread_cond_t cond = PTHREAD_COND_INITIALIZER;
pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
int tickets = 100;

void* start_routine(void* args) {
    string name = static_cast<const char*>(args);
    // what's pthread do
    while (true) {
        pthread_mutex_lock(&mutex);
        pthread_cond_wait(&cond, &mutex);
        cout << name << " -> " << tickets;
        tickets--;
        pthread_mutex_unlock(&mutex);
    }
    return nullptr;
}

int main() {
    pthread_t p1, p2;
    pthread_create(&p1, nullptr, start_routine, (void*)"thread1");
    pthread_create(&p1, nullptr, start_routine, (void*)"thread2");

    while (true) {
        sleep(1);
        pthread_cond_signal(&cond);
        cout << "main thread wakeup a thread" << endl;
    }

    pthread_join(p1, nullptr);
    pthread_join(p1, nullptr);
    return 0;
}