#include <iostream>
#include <pthread.h>
#include <unistd.h>



// using namespace std;

int tickets = 1000;

pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t cond = PTHREAD_COND_INITIALIZER;

void* start_routine(void* args) {
    char* threadname = static_cast<char*>(args);
    while(true) {
        // 加锁
        pthread_mutex_lock(&mutex);
        // 等待条件满足
        pthread_cond_wait(&cond, &mutex);

        // 处理
        std::cout << threadname << " -> " << tickets-- << std::endl;

        // 解锁
        pthread_mutex_unlock(&mutex);
    }
}

int main() {
    pthread_t p1,p2,p3;
    pthread_create(&p1, nullptr, start_routine, (void*)"thread-1");
    pthread_create(&p2, nullptr, start_routine, (void*)"thread-2");

    while(true) {
        sleep(1);
        pthread_cond_signal(&cond);
    }

    pthread_join(p1, nullptr);
    pthread_join(p2, nullptr);

    return 0;
}