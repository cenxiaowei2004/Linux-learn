#include <iostream>
#include <pthread.h>
#include <unistd.h>
#include <cstring>
#include <vector>
#include <cassert>

using namespace std;

// void* start_routine(void* args) {
//     std::string name = static_cast<const char*>(args);
//     while(true) {
//         std::cout << "--新线程-- name: " << name << std::endl;
//         sleep(1);
//         int* ptr = nullptr;
//         *ptr = 0;
//     }
// }

// int main() {
//     pthread_t pth = 1;
//     int n = pthread_create(&pth, nullptr, start_routine, (void*)"thread one");
//     while(true) {
//         std::cout << "--主线程--" << std::endl;
//         sleep(1);
//     }
//     return 0;
// }

// class ThreadData {
// public:
//     int number;
//     pthread_t pid;
//     char buffername[32];
//     // ThreadData(pthread_t _pid) : pid(_pid) {};
// };

// const int NUM = 10;
// std::vector<ThreadData*> Threaddatas;

// void* start_routine(void* args) {
//     sleep(1);
//     ThreadData* thd = static_cast<ThreadData*>(args);
//     std::cout << "新线程 name: " << thd->buffername << std::endl;
//     // delete thd;
//     // 终止进程
//     pthread_exit((void*)thd->number);
//     return nullptr;
// }


// // 批量生成线程:
// int main() {
//     for(int i = 1; i <= NUM; i++) {    
//         ThreadData* thd = new ThreadData();
//         Threaddatas.push_back(thd);
//         thd->number = i;
//         snprintf(thd->buffername, sizeof(thd->buffername), "thread:%d", i);
//         // 创建进程
//         pthread_t phd = pthread_create(&thd->pid, nullptr, start_routine, thd);
//         // sleep(1);
//     }
//     for(auto& thread : Threaddatas) {
//         cout << thread->buffername << endl;
//     }
//     for(auto& thread : Threaddatas) {
//         // 等待进程
//         void* ret = nullptr;
//         int n = pthread_join(thread->pid, &ret);
//         assert(n == 0);
//         cout << thread->buffername << " | " << (long long)ret << "  join success!" << endl;
//         delete thread;
//     }
//     // while(true) {
//     //     std::cout << "主线程" << std::endl;
//     //     sleep(1);
//     // }
//     cout << "main thread quit" << endl;
//     return 0;
// }


// int main() {
//     int a = 100;
//     int* ptr = &a;
//     cout << sizeof(ptr) << endl;
//     return 0;
// }

string getID(const pthread_t& tid) {
    char buffer[32];
    snprintf(buffer, sizeof(buffer), "0x%x", tid);
    return buffer;
}

void* start_routine(void* args) {
    string name = static_cast<const char*>(args);
    // pthread_detach(pthread_self());

    while(true) {
        std::cout << "new thread" << name << getID(pthread_self()) << "  running..." << std::endl;
        sleep(1);
    }
    return nullptr;
}

int main() {
    pthread_t tid;
    pthread_create(&tid, nullptr, start_routine, (void*)"thread");
    pthread_detach(tid);
    
    cout << "main thread running..." << endl;
    int n = pthread_join(tid, nullptr);
    cout << n << strerror(n) << endl;
    return 0;
}