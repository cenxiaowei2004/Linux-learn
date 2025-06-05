#include <iostream>
#include <time.h>
#include <unistd.h>

using namespace std;


// 互斥量及初始化
pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER; 

int glo_val = 1000;

void* a_thread(void* args) {
    // cout << "a thread..." << endl;
    while(true) {
        pthread_mutex_lock(&mutex);
        glo_val = 200;
        cout << "a:" << glo_val<< endl;
        sleep(1);
        pthread_mutex_unlock(&mutex);
    }
    return nullptr;
}

void* b_thread(void* args) {
    // cout << "b thread..." << endl;
    while(true) {
        pthread_mutex_lock(&mutex);
        glo_val = 100;
        cout << "b:" << glo_val<< endl;
        sleep(1);
        pthread_mutex_unlock(&mutex);
    }
    return nullptr;
}


int main() {
    // pthread_t a, b;
    // pthread_create(&a, nullptr, a_thread, (void*)"a");
    // pthread_create(&a, nullptr, b_thread, (void*)"b");
    // pthread_join(a, nullptr);
    // sleep(1);
    // pthread_join(b, nullptr);
    // pthread_mutex_destroy(&mutex);
    
    /**
    * 互斥量？通过 lock/unlock 控制多个线程对共享资源的访问，保证一次只有一个线程操作资源。（用来保证同步性，保护共享资源）
    * 实际的可能存在的情况是：一份公共资源可能允许访问不同的区域
    * 所以我们先申请信号量，从而提前知道临界资源的使用情况，故而信号量必须是公共资源
    * 信号量是计数器，递增/递减，且必须保证原子性
    * sem_t sem: 
    * 1、++ 释放资源  V 操作
    * 2、-- 申请资源  P 操作
    * 核心：PV原语
    * */

    // 引入环形队列




    


    return 0;
}
