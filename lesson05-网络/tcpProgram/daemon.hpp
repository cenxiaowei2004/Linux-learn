#pragma once

#include <fcntl.h>
#include <signal.h>
#include <stdlib.h>
#include <unistd.h>

void daemonSelf() {
    // 1. 忽略异常信号
    signal(SIGPIPE, SIG_IGN);
    // 2. 让非组织进程调用 setsid
    if (fork() > 0)
        exit(0);
    // 子进程：守护进程/精灵进程，属于孤儿进程
    setsid();
    // 3. 重定向进程默认的文件
    int fd = open("/dev/null", O_RDWR | O_CREAT, 0666);
    if (fd >= 0) {
        dup2(fd, 0);
        dup2(fd, 1);
        dup2(fd, 2);
        close(fd);
    } else {
        close(0);
        close(1);
        close(2);
    }
}