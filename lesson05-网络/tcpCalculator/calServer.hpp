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

typedef function<void(const Request& req, Response& resp)> func_t;

static const int backlog = 5;

class calServer;

class calServerData {
public:
    calServerData(calServer* _server, int& _sock, func_t& _func) : server(_server), sock(_sock), func(_func) {}

    calServer* server;
    int sock;
    func_t func;
};

class calServer {
private:
    static void* DataIO(void* args) {
        static string inbuffer;
        calServerData* data = static_cast<calServerData*>(args);
        int sock = data->sock;
        func_t func = data->func;
        while (true) {
            Request req;
            Response resp;

            string req_str, resp_str, req_text;
            // 【服务端】
            // 1. 读取（读取一个完整的请求）
            // "content_len\r\n""num1 op num2\n\r"
            if (!recvRequset(sock, inbuffer, &req_text)) {
                break;
            }

            cout << "带包头的报文:" << req_text << endl;

            // "num1 op num2\n\r"
            if (!delLength(req_text, &req_str)) {
                break;
            }

            cout << "去掉报头的报文:" << req_str << endl;

            // "num1 op num2\n\r" -> num1 num2 op...
            // 2. 请求反序列化（得到一个结构化的请求）deserialize
            if (!req.deserialize(req_str)) {
                break;
            }

            // 3. 处理结果
            func(req, resp);
            // 4. 响应序列化（得到一个结构化的响应） serialize
            resp.serialize(&resp_str);

            cout << "序列化后的响应:" << resp_str << endl;

            string send_str = addLength(resp_str);
            // 5. 返回响应

            cout << "返回的响应:" << send_str << endl;

            send(sock, send_str.c_str(), send_str.size(), 0);
        }
        close(sock);
        delete data;
        return nullptr;
    }

public:
    calServer(const uint16_t& _port) : port(_port), listenfd(-1) {}

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
            calServerData* data = new calServerData(this, sock, func);
            pthread_create(&pid, nullptr, DataIO, (void*)data);
            pthread_detach(pid);
        }
    }

    ~calServer() {
        if (listenfd != -1) {
            close(listenfd);
            listenfd = -1;
        }
    }

private:
    uint16_t port;
    int listenfd; /* 监听套接字 */
};
}  // namespace Server