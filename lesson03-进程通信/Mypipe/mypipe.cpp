#include <iostream>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <cstdio>
#include <cstring>

using namespace std;
// int main() {
//     // 创建管道文件，打开读写端
//     int fds[2];
//     int ret = pipe(fds);
//     cout << "fds[0] = " << fds[0] << endl; // [0]:读取->3
//     cout << "fds[1] = " << fds[1] << endl; // [1]:写入->4
//     // 父进程进行读取，子进程进行写入
//     // 创建子进程
//     pid_t id = fork();
//     if(id == -1)
//         return 0;
//     if (id == 0) {
//         // 子进程
//         // 子进程通信代码
//         close(fds[0]);
//         int cnt = 0;
//         // 子进程每隔1s进行写入
//         while (true) {
//             cnt++;
//             char buff[1024];
//             snprintf(buff, sizeof(buff), "%d:%d :msg from child process to father process", cnt, getpid());
//             write(fds[0], buff, strlen(buff));
//             sleep(1);
//         }
//         close(fds[1]);
//         exit(0);
//     }
//     // 父进程
//     close(fds[0]);
//     // 父进程读取
//     while (true) {
//         char buff[1024];
//         ssize_t sz = read(fds[1], buff, sizeof(buff) - 1);
//         if (sz > 0) {
//             buff[sz] = '\0'; // 添加字符串结束符
//             cout << "#" << getpid() << " | get message: " << buff << endl;
//         } else if (sz == 0) {
//             // 管道写端被关闭，退出循环
//             cout << "Child process closed the pipe. Exiting..." << endl;
//             break;
//         } else {
//             perror("read failed");
//             break;
//         }
//     }
//     close(fds[0]);
//     int n = waitpid(id, nullptr, 0);
    
//     return 0;
// }

#include <iostream>
#include <unistd.h>
#include <cstring>
#include <cstdio>
#include <cstdlib>

using namespace std;

int main() {
    // 创建管道
    int fds[2];
    if (pipe(fds) == -1) {
        perror("pipe failed");
        return 1;
    }

    cout << "fds[0] (read end) = " << fds[0] << endl; // 读端
    cout << "fds[1] (write end) = " << fds[1] << endl; // 写端

    // 创建子进程
    pid_t id = fork();

    if (id == -1) {
        perror("fork failed");
        return 1;
    }

    if (id == 0) {
        // 子进程
        close(fds[0]); // 关闭读端

        int cnt = 0;
        while (cnt < 5) { // 设置最大消息数为5
            cnt++;
            char buff[1024];
            snprintf(buff, sizeof(buff), "%d:%d :msg from child process to father process", cnt, getpid());
            write(fds[1], buff, strlen(buff)); // 向管道写端写入数据
            sleep(3); // 每隔3秒发送一次消息
        }
        close(fds[1]); // 关闭写端
        exit(0); // 子进程退出
    } else {
        // 父进程
        close(fds[1]); // 关闭写端

        while (true) {
            char buff[1024];
            ssize_t sz = read(fds[0], buff, sizeof(buff) - 1); // 从管道读端读取数据
            if (sz > 0) {
                buff[sz] = '\0'; // 添加字符串结束符
                cout << "#" << getpid() << " | get message: " << buff << endl;
            } else if (sz == 0) {
                // 管道写端被关闭，退出循环
                cout << "Child process closed the pipe. Exiting..." << endl;
                break;
            } else {
                perror("read failed");
                break;
            }
        }

        close(fds[0]); // 关闭读端
        waitpid(id, nullptr, 0); // 等待子进程退出
    }

    return 0;
}
