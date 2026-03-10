
#include <iostream>
using namespace std;

// 进程管理：
// 创建进程：fork
//

#include <pwd.h>
#include <sys/wait.h>
#include <unistd.h>
#include <cstring>

// int global_val = 1000;

// int main() {
//     // 创建子进程
//     // 写时拷贝
//     pid_t pid = fork();
//     if (pid == 0) {
//         // child thread:
//         cout << "child..." << endl;
//         cout << "child:" << global_val << endl;
//     } else if (pid > 0) {
//         // father thread:
//         cout << "father..." << endl;
//         global_val = 100;
//         cout << "father:" << global_val << endl;
//     } else {
//     }  // fork fail
//     return 0;
// }

// 简易shell
void SimpleShell(int argc, char* argv[]) {
    int const MAX_NUM = 64;
    int const STR_NUM = 10;
    char cmd[MAX_NUM];
    char hostname[32];
    char directory[32];
    char* strs[STR_NUM];
    char* sprit;
    while (true) {
        struct passwd* pass = getpwuid(getuid());
        gethostname(hostname, sizeof(hostname) - 1);
        getcwd(directory, sizeof(directory) - 1);
        // 截取最后一个/Linux:
        sprit = directory + strlen(directory);
        while (--sprit) {
            if (*sprit == '/')
                break;
        }
        sprit++;
        printf("[%s@%s %s]$ ", pass->pw_name, hostname, sprit);
        fgets(cmd, sizeof(cmd), stdin);
        cmd[strlen(cmd) - 1] = '\0';  // 移除换行符
        // 1. 分割命令
        int index = 0;
        strs[0] = strtok(cmd, " ");
        while (strs[index] != nullptr) {
            strs[++index] = strtok(nullptr, " ");
        }

        // 2. 创建子进程
        pid_t pid = fork();
        if (pid == 0) {
            // 子进程
            // 3. 替换函数
            execvp(strs[0], strs);
            exit(1);
        }
        // 父进程
        int status = 0;
        pid_t ret = waitpid(pid, &status, 0);
        if (ret > 0) {
            printf("exit code:%d\n", WEXITSTATUS(status));
        }
    }
}

// 匿名管道：用于进程间通信，且仅限于本地父子进程之间的通信。
void TestPipe() {
    int fds[2];
    int ret = pipe(fds);
    if (ret < 0) {
        cout << "pipe create fail.";
        return;
    }
    cout << "fds[0] (read end) = " << fds[0] << endl;   // 读端
    cout << "fds[1] (write end) = " << fds[1] << endl;  // 写端

    pid_t pid = fork();
    if (pid == 0) {
        // child:
        cout << "child" << endl;
        while (true) {
            sleep(2);
        }
    } else if (pid > 0) {
        // father:
        cout << "father" << endl;
    }
}

#include <pthread.h>

// 线程控制
void* DoTask(void* argv) {
    while (true) {
        std::cout << "I am sub_thread, pid:" << getpid() << " ppid:" << getppid() << std::endl;

        sleep(1);
    }
    return nullptr;
}

//     PID     LWP TTY          TIME CMD
//  229043  229043 pts/3    00:00:00 Main
//  229043  229044 pts/3    00:00:00 Main
// Linux中的线程即是轻量级的进程，可见两个线程LWP229043和229044同属于进程PID229043
// pid:进程ID/ppid:父进程ID
void TestThread() {
    // typedef unsigned long int pthread_t;
    pthread_t p;
    pthread_create(&p, nullptr, DoTask, (void*)"thread-one");
    std::cout << p << std::endl;
    while (true) {
        std::cout << "I am main_thread, pid:" << getpid() << " ppid:" << getppid() << std::endl;
        sleep(1);
    }
    pthread_join(p, nullptr);
}

pthread_cond_t cond;
pthread_mutex_t mutex;

void* Runing(void* argv) {
    // char* buf = (char*)argv;
    printf("subthread: I am %s, pid: %d, ppid:%d\n", (char*)argv, getpid(), getppid());
    pthread_mutex_lock(&mutex);
    while (true) {
        std::cout << "阻塞中..." << std::endl;
        pthread_cond_wait(&cond, &mutex);
        std::cout << "活动中..." << std::endl;
    }
    pthread_mutex_unlock(&mutex);
    return nullptr;
}

// 下面我们用主线程创建三个新线程，让主线程控制这三个新线程活动
// 这三个新线程创建后都在条件变量下进行等待，直到主线程检测到键盘有输入时才唤醒一个等待线程
void TestCv() {
    pthread_t pts[3];
    pthread_mutex_init(&mutex, nullptr);
    pthread_cond_init(&cond, nullptr);

    pthread_create(&pts[0], nullptr, Runing, (void*)"thread-0");
    pthread_create(&pts[1], nullptr, Runing, (void*)"thread-1");
    pthread_create(&pts[2], nullptr, Runing, (void*)"thread-2");
    // ...
    while (true) {
        // 检测键盘输入：
        printf("mainthread: pid: %d, ppid:%d\n", getpid(), getppid());
        getchar();
        pthread_cond_broadcast(&cond);
    }

    for (int i = 0; i < 3; ++i) {
        pthread_join(pts[i], nullptr);
    }

    // clear:
    pthread_mutex_destroy(&mutex);
    pthread_cond_destroy(&cond);
}

/**
 * 线程安全：
 * 多个线程并发同一段代码时，不会出现不同的结果。常见对全局变量或者静态变量进行操作，并且没有锁保护的情况下，会出现线程安全问题。
 * 重入： 同一个函数被不同的执行流调用，当前一个流程还没有执行完，就有其他的执行流再次进入，我们称之为重入。
 * 一个函数在重入的情况下，运行结果不会出现任何不同或者任何问题，则该函数被称为可重入函数，否则是不可重入函数。
 */

/*
临界资源：多线程共享的资源
临界区：访问临界资源的代码
原子性：做 or 不做一条汇编完成
互斥：任何时刻，保证有且只有一个执行流进入临界区
同步：在保证数据安全的前提下，让线程能够按照某种特定的顺序访问临界资源，从而有效避免饥饿问题
竞态条件：因为时序问题，而导致程序异常
*/
// 为了保证互斥，Linux提供了互斥锁mutex
// 为了保证同步，提供了pthread_cond，与互斥量一起使用
// 条件变量是用来描述某种资源是否就绪的一种数据化描述

#include "client.hpp"

int main(int argc, char* argv[]) {
    // TestPipe();
    // TestThread();
    // TestCv();
    // char c = getchar();
    // cout << c;

    // select/poll/epoll:
    // ...
    client c;
    c.Run();
    return 0;
}