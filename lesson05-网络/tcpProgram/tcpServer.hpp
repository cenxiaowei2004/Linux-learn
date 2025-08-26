
#include <arpa/inet.h>
#include <errno.h>
#include <strings.h>
#include <sys/socket.h>
#include <unistd.h>

#include "log.hpp"

#include <cstdlib>
#include <cstring>
#include <functional>
#include <iostream>

namespace Server {

using namespace std;

enum { USAGE_ERR = 1, SOCKET_ERR, BIND_ERR, OPEN_ERR, LISTEN_ERR, ACCEPT_ERR };

typedef function<void(int, string, uint16_t, string)> func;

static const int backlog = 5;

class tcpServer {
private:
    void DataIO(int sock) {
        char buffer[1024];
        while (true) {
            // 读
            ssize_t n = read(sock, buffer, sizeof(buffer) - 1);
            if (n == 0) {
                logMessage(NORMAL, "client quit");
                break;
            }
            buffer[n] = 0;
            cout << "receive message:" << buffer << endl;
            ;
            string response = buffer;
            response += "[server send]";
            // 写
            write(sock, response.c_str(), response.size());
        }
    }

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
        while (true) {
            // 4. server 获取新链接
            struct sockaddr_in peer;
            socklen_t len = sizeof(peer);

            int sock = accept(listenfd, (struct sockaddr*)&peer, &len);
            if (sock < 0) {
                logMessage(ERROR, "accept socket error!");
                exit(ACCEPT_ERR);
            }
            
            cout << "listenfd:" << listenfd << endl;
            cout << "sock:" << sock << endl;
            
            // 5. TCP：面向字节流，文件操作
            DataIO(sock);
            // 要关闭已经使用过的 sock，否则会导致文件描述符泄露
            close(sock);
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
