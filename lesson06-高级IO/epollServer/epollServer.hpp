#pragma once

#include <functional>
#include <vector>

#include <sys/epoll.h>

#include "log.hpp"
#include "socket.hpp"

using namespace std;

typedef function<string(char*)> func_t;

namespace Server {

const static int eventnum = 10;
const static int timeout = 3000;

class epollServer {
private:
    void Accepter() {
        string clientip;
        uint16_t clientport;
        int sock = Sock::Accept(listensock, &clientip, &clientport);
        if (sock < 0) {
            logMessage(WARNING, "accept fail");
        }
        logMessage(NORMAL, "have a new link [%s : %d], sock: %d", clientip.c_str(), clientport, sock);
        struct epoll_event ev;
        ev.events = EPOLLIN;
        ev.data.fd = sock;
        epoll_ctl(epfd, EPOLL_CTL_ADD, sock, &ev);
    }

    void Recver(int sock) {
        logMessage(NORMAL, "Recver in");
        char buffer[1024];
        ssize_t n = recv(sock, buffer, sizeof(buffer) - 1, 0);
        // 你怎么保证读的是一个报文？不能保证
        if (n <= 0) {
            logMessage(WARNING, "client quit");
            // 建议：先 epoll 移除后 close 关闭
            epoll_ctl(epfd, EPOLL_CTL_DEL, sock, nullptr);
            close(sock);
        } else {
            buffer[n] = 0;
            string resp = func(buffer);
            logMessage(NORMAL, "client# %s", buffer);
            send(sock, resp.c_str(), resp.size(), 0);
        }
        logMessage(NORMAL, "Recver off");
    }

    void handlerEvent(int n) {
        for (int i = 0; i < n; i++) {
            uint32_t event = events[i].events;
            int sock = events[i].data.fd;
            if (sock == listensock && (event & EPOLLIN)) {
                // listen 套接字就绪
                Accepter();
            } else if (event & EPOLLIN) {
                // 普通套接字就绪
                Recver(sock);
            }
        }
    }

public:
    epollServer(uint16_t _port, func_t _func) : port(_port), listensock(-1), func(_func), epfd(-1), events(nullptr) {}

    void init() {
        // 1. 创建 socket
        listensock = Sock::Socket();
        Sock::Bind(listensock, port);
        Sock::Listen(listensock);
        // 2. 创建 epoll 模型
        epfd = epoll_create(256);
        if (epfd < 0) {
            logMessage(FATAL, "epoll_create error!");
            exit(EPOLL_CREATE_ERR);
        }
        // 3. 将 listen 套接字的读事件关心起来
        struct epoll_event ev;
        ev.events = EPOLLIN;
        ev.data.fd = listensock;  // 当事件就绪时，可以知道是哪一个 fd
        epoll_ctl(epfd, EPOLL_CTL_ADD, listensock, &ev);
        // 4. 存放就绪时间的数组
        events = new struct epoll_event[eventnum];
    }

    void start() {
        while (true) {
            // 5. 得到就绪的时间
            int n = epoll_wait(epfd, events, eventnum, timeout);
            switch (n) {
                case 0:
                    logMessage(NORMAL, "timeout...");
                    break;
                case -1:
                    logMessage(ERROR, "%s", strerror(errno));
                    break;
                default:
                    logMessage(DEBUG, "have %d links", n);
                    handlerEvent(n);
                    break;
            }
        }
    }

    ~epollServer() {
        close(listensock);
        close(epfd);
        listensock = -1;
        epfd = -1;
        delete[] events;
    }

private:
    uint16_t port;
    int listensock;
    func_t func;
    int epfd;
    struct epoll_event* events;
};

}  // namespace Server
