#pragma once

#include <arpa/inet.h>
#include <pthread.h>
#include <sys/socket.h>
#include <sys/types.h>

#include "Task.hpp"
#include "ThreadPool.hpp"
#include "log.hpp"

#include <errno.h>
#include <strings.h>
#include <unistd.h>
#include <cstdlib>
#include <cstring>
#include <functional>
#include <iostream>

namespace Server {

using namespace std;

enum { USAGE_ERR = 1, SOCKET_ERR, BIND_ERR, OPEN_ERR, LISTEN_ERR, ACCEPT_ERR, FORK_ERR };

typedef function<void(int, string, uint16_t, string)> func;

static const int backlog = 5;

class tcpServer;

class tcpServerData {
public:
    tcpServerData(tcpServer* _server, const int _sock) : server(_server), sock(_sock) {}

public:
    tcpServer* server;
    int sock;
};

class tcpServer {
private:
    // static void* clientThread(void* args) {
    //     tcpServerData* serverData = static_cast<tcpServerData*>(args);
    //     serverData->server->DataIO(serverData->sock);
    //     delete serverData;
    //     close(serverData->sock);
    //     return nullptr;
    // }

public:
    tcpServer(const uint16_t& _port, const func& _callback) : port(_port), callback(_callback), listenfd(-1) {}

    void initServer() {
        // 1. 创建 socket 套接字
        int n = 0;
        listenfd = socket(AF_INET, SOCK_STREAM, 0);
        if (listenfd < 0) {
            logMessage(FATAL, "create socket fail!");
            exit(SOCKET_ERR);
        }

        // 2. 绑定端口号
        struct sockaddr_in local;
        local.sin_family = AF_INET;
        local.sin_addr.s_addr = INADDR_ANY;
        local.sin_port = htons(port);

        n = bind(listenfd, (struct sockaddr*)&local, static_cast<socklen_t>(sizeof(local)));
        if (n < 0) {
            logMessage(FATAL, "bind internet error!");
            exit(BIND_ERR);
        }

        // 3. 设置 socket 为监听状态
        n = listen(listenfd, backlog);
        if (n < 0) {
            logMessage(FATAL, "listen socket error!");
            exit(LISTEN_ERR);
        }
    }

    void start() {
        // v3:线程池版本
        ThreadPool<Task>::getInstance()->run();

        while (true) {
            // 4. server 获取新链接
            struct sockaddr_in peer;
            socklen_t len = sizeof(peer);

            int sock = accept(listenfd, (struct sockaddr*)&peer, &len);
            if (sock < 0) {
                logMessage(ERROR, "accept socket error!");
                exit(ACCEPT_ERR);
            }

            logMessage(NORMAL, "accept socket success!");
            cout << "listenfd:" << listenfd << endl;
            cout << "sock:" << sock << endl;

            // 5. TCP：面向字节流，文件操作

            // v1:基础版本
            // DataIO(sock);
            // 要关闭已经使用过的 sock，否则会导致文件描述符泄露
            // close(sock);

            // v2：多线程版本
            // pthread_t pid;
            // tcpServerData* serverData = new tcpServerData(this, sock);
            // pthread_create(&pid, nullptr, clientThread, (void*)serverData);
            // pthread_detach(pid);
            // 推送任务
            Task task(sock);
            ThreadPool<Task>::getInstance()->push(task);
        }
    }

    ~tcpServer() {
        if (listenfd != -1) {
            close(listenfd);
            listenfd = -1;
        }
    }

private:
    uint16_t port;
    int listenfd;  // 这个套接字不是用来数据通信的，而是用来监听链接，获取新链接的

    func callback;
};
}  // namespace Server
