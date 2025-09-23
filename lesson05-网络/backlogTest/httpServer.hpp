#pragma once

#include <arpa/inet.h>
#include <pthread.h>
#include <sys/socket.h>
#include <sys/types.h>

#include "log.hpp"
#include "protocol.hpp"

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

typedef function<bool(const HttpRequest& req, HttpResponse& resp)> func_t;

static const int backlog = 2;

class httpServer;

class httpServerData {
public:
    httpServerData(httpServer* _server, int& _sock, const func_t& _func) : server(_server), sock(_sock), func(_func) {}

    httpServer* server;
    int sock;
    func_t func;
};

class httpServer {
private:
    static void* DataIO(void* args) {
        static string inbuffer;
        httpServerData* data = static_cast<httpServerData*>(args);
        int sock = data->sock;
        func_t func = data->func;
        char buffer[4096];
        HttpRequest req;
        HttpResponse resp;
        while (true) {
            // 【服务端】
            // 1. 读取（读取一个完整的请求）
            ssize_t n = recv(sock, buffer, sizeof(buffer) - 1, 0);
            if (n <= 0)
                break;

            buffer[n] = 0;
            req.inbuffer = buffer;
            // 2. 请求反序列化（得到一个结构化的请求）
            // 3. 处理结果
            req.parse();
            func(req, resp);
            // 4. 响应序列化（得到一个结构化的响应）
            send(sock, resp.outbuffer.c_str(), resp.outbuffer.size(), 0);
            // 5. 返回响应
        }
        close(sock);
        delete data;
        return nullptr;
    }

public:
    httpServer(const uint16_t& _port) : port(_port), listenfd(-1) {}

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

    void start(func_t func) {
        while (true) {
            sleep(1);
            /*
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

            pthread_t pid;
            httpServerData* data = new httpServerData(this, sock, func);
            pthread_create(&pid, nullptr, DataIO, (void*)data);
            pthread_detach(pid);
            */
        }
    }

    ~httpServer() {
        if (listenfd != -1) {
            close(listenfd);
            listenfd = -1;
        }
    }

private:
    uint16_t port;
    int listenfd;
};
}  // namespace Server