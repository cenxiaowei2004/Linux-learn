#include <unistd.h>
#include "Blockqueue.hpp"

void* consumer(void* args) {
    while (true) {
        BlockQueue<int>* bq = static_cast<BlockQueue<int>*>(args);
        // consumer activities:
        int data;
        bq->pop(&data);
        cout << "consumer data:" << data << endl;
    }
    return nullptr;
}

void* productor(void* args) {
    while (true) {
        BlockQueue<int>* bq = static_cast<BlockQueue<int>*>(args);
        // productor activities:
        int data = rand() % 10 + 1;
        bq->push(data);
        cout << "productor data:" << data << endl;
    }
    return nullptr;
}

/**
 * 1. 什么是信号量？
 * 信号量本质是一把计数器，衡量临界资源中资源数量多少的计数器，只要拥有信号量就拥有了临界资源的一部分
 * 2. 为什么存在信号量？
 * 为了预先知道临界资源的使用情况
 * 3. PV原语？
 * ++：归还资源 -- P操作
 * --：申请资源 -- V操作
 */

int main() {
    srand((unsigned long)time(nullptr) ^ getpid());
    BlockQueue<int>* bq = new BlockQueue<int>(5);

    pthread_t c, p;
    pthread_create(&c, nullptr, consumer, bq);
    pthread_create(&p, nullptr, productor, bq);

    pthread_join(c, nullptr);
    pthread_join(p, nullptr);

    delete bq;
    return 0;
}