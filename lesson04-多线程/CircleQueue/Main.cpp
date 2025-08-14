#include <unistd.h>
#include <cstdlib>
#include <ctime>
#include "Circlequeue.hpp"

string setName() {
    char buffer[32];
    snprintf(buffer, sizeof(buffer), "thread[0x%x]", pthread_self());
    return buffer;
}

void* consumer(void* args) {
    CircleQueue<int>* cq = static_cast<CircleQueue<int>*>(args);
    while (true) {
        int data;
        cq->pop(&data);
        cout << setName() << "consumer -- data:" << data << endl;
    }
}

void* productor(void* args) {
    CircleQueue<int>* cq = static_cast<CircleQueue<int>*>(args);
    while (true) {
        int data = rand() % 10 + 1;
        cq->push(data);
        cout << setName() << "productor -- data:" << data << endl;
    }
}

// multiple consumer + multiple productor:
int main() {
    srand((unsigned long)time(nullptr) ^ getpid());
    CircleQueue<int>* cq = new CircleQueue<int>(5);

    pthread_t p[3], c[3];
    for (auto& i : p) {
        pthread_create(&i, nullptr, productor, cq);
    }

    for (auto& i : c) {
        pthread_create(&i, nullptr, consumer, cq);
    }

    for (auto& i : p)
        pthread_join(i, nullptr);

    for (auto& i : c)
        pthread_join(i, nullptr);

    delete cq;
    return 0;
}