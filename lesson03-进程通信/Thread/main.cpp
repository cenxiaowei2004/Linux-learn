/*
#include "Thread.hpp"
#include <memory>
#include <unistd.h>
#include <vector>

// 没有分号哦
#define UserNum 4


class ThreadData {
public:
    ThreadData(const string& name, pthread_mutex_t* _mutex) : threadname(name), mutex(_mutex) {}
    ~ThreadData() {}
public:
    string threadname;
    pthread_mutex_t* mutex;
};

// 共享资源：
int tickets = 1000;

// pthread_mutex_t lock = PTHREAD_MUTEX_INITIALIZER;

// 抢票函数：
void* thread_run(void* args) {
    // string username = static_cast<const char*>(args);
    // while (true) {
    //     if(tickets == 888) {
    //         usleep(1000000009);
    //     }
    //     else
    //         usleep(1000);
    //     if(tickets > 0) {
    //         cout << username << "正在抢票..." << tickets << endl;
    //         tickets--;
    //     }
    //     else {
    //         break;
    //     }
    // }
    // char* username = static_cast<char*>(args);
    // 加锁
    // pthread_mutex_lock(&lock);
    
    ThreadData* td = static_cast<ThreadData*>(args);

    while (true) {
        pthread_mutex_lock(td->mutex);
        usleep(10000);
        if(tickets > 0) {
            // printf("%s 正在抢票: %d\n", td->threadname, tickets);
            cout << td->threadname << "正在抢票:" << tickets << endl;
            tickets--;
            // 解锁
            pthread_mutex_unlock(td->mutex);   
        } else {
            pthread_mutex_unlock(td->mutex);   
            break;
        }
        usleep(1000);
    }
    return nullptr;
}

int main() {
    pthread_mutex_t lock;
    vector<pthread_t> pids(UserNum);
    pthread_mutex_init(&lock, nullptr);

    
    // 方式1：
    for(int i = 0; i < UserNum; i++) {
        char buffer[64];
        snprintf(buffer, sizeof buffer, "thread-%d", i + 1);
        ThreadData* td = new ThreadData(buffer, &lock);
        pthread_create(&pids[i], nullptr, thread_run, td); 
    }

    for(auto& pid : pids) {
        pthread_join(pid, nullptr);
    }

    pthread_mutex_destroy(&lock);




    // pthread_t t1,t2,t3,t4;
    // pthread_create(&t1, nullptr, thread_run, (void*)"thread-1");
    // pthread_create(&t2, nullptr, thread_run, (void*)"thread-2");
    // pthread_create(&t3, nullptr, thread_run, (void*)"thread-3");
    // pthread_create(&t4, nullptr, thread_run, (void*)"thread-4");

    // unique_ptr<Thread> t1(new Thread(thread_run, (void*)"user1", 1));
    // unique_ptr<Thread> t2(new Thread(thread_run, (void*)"user2", 2));
    // unique_ptr<Thread> t3(new Thread(thread_run, (void*)"user3", 3));
    // unique_ptr<Thread> t4(new Thread(thread_run, (void*)"user4", 4));
    // t1->join();
    // t2->join();
    // t3->join();
    // t4->join();

    return 0;
}

*/

/*
#include <pthread.h>
#include <unistd.h>
#include <vector>
#include <iostream>
#include "Mutex.hpp"

using namespace std;

// 没有分号哦
#define UserNum 4


class ThreadData {
public:
    ThreadData(const string& name, pthread_mutex_t* _mutex) : threadname(name), mutex(_mutex) {}
    ~ThreadData() {}
public:
    string threadname;
    pthread_mutex_t* mutex;
};

// 共享资源：
int tickets = 1000;

// 抢票函数：
void* thread_run(void* args) {
    ThreadData* td = static_cast<ThreadData*>(args);
    while (true) {
        pthread_mutex_lock(td->mutex);
        usleep(10000);
        if(tickets > 0) {
            cout << td->threadname << "正在抢票:" << tickets << endl;
            tickets--;
            // 解锁
            pthread_mutex_unlock(td->mutex);   
        } else {
            pthread_mutex_unlock(td->mutex);   
            break;
        }
        usleep(1000);
    }
    return nullptr;
}

int main() {
    pthread_mutex_t lock;
    vector<pthread_t> pids(UserNum);
    pthread_mutex_init(&lock, nullptr);

    for(int i = 0; i < UserNum; i++) {
        char buffer[64];
        snprintf(buffer, sizeof buffer, "thread-%d", i + 1);
        ThreadData* td = new ThreadData(buffer, &lock);
        pthread_create(&pids[i], nullptr, thread_run, td); 
    }

    for(auto& pid : pids) {
        pthread_join(pid, nullptr);
    }

    pthread_mutex_destroy(&lock);
    return 0;
}
*/


/*
#include <pthread.h>
#include <unistd.h>
#include <vector>
#include <iostream>

using namespace std;

// 没有分号哦
#define UserNum 4

class ThreadData {
public:
    ThreadData(const string& name) : threadname(name) {}
    ~ThreadData() {}
public:
    string threadname;
};

// 共享资源：
int tickets = 1000;

// 抢票函数：
void* thread_run(void* args) {
    ThreadData* td = static_cast<ThreadData*>(args);
    while (true) {
        usleep(10000);
        if(tickets > 0) {
            cout << td->threadname << "正在抢票:" << tickets << endl;
            tickets--; 
        } else { 
            break;
        }
        usleep(1000);
    }
    return nullptr;
}

int main() {
    vector<pthread_t> pids(UserNum);
    for(int i = 0; i < UserNum; i++) {
        char buffer[64];
        snprintf(buffer, sizeof buffer, "thread-%d", i + 1);
        ThreadData* td = new ThreadData(buffer);
        pthread_create(&pids[i], nullptr, thread_run, td); 
    }

    for(auto& pid : pids) {
        pthread_join(pid, nullptr);
    }
    return 0;
}
*/

#include <pthread.h>
#include <unistd.h>
#include <vector>
#include <iostream>
#include "Mutex.hpp"

using namespace std;

// 没有分号哦
#define UserNum 4

pthread_mutex_t lock = PTHREAD_MUTEX_INITIALIZER;

// 共享资源：
int tickets = 1000;

// 抢票函数：
void* thread_run(void* args) {
    char* threadname = (char*) args;
    while (true) {
        LockGuard lockguard(&lock);
        usleep(10000);
        if(tickets > 0) {
            cout << threadname << "正在抢票:" << tickets << endl;
            tickets--;
        } else {
            break;
        }
        usleep(1000);
    }
    return nullptr;
}

int main() {
    pthread_mutex_t lock;
    vector<pthread_t> pids(UserNum);
    pthread_mutex_init(&lock, nullptr);

    for(int i = 0; i < UserNum; i++) {
        char buffer[64];
        snprintf(buffer, sizeof buffer, "thread-%d", i + 1);
        pthread_create(&pids[i], nullptr, thread_run, buffer); 
    }

    for(auto& pid : pids) {
        pthread_join(pid, nullptr);
    }

    pthread_mutex_destroy(&lock);
    return 0;
}


