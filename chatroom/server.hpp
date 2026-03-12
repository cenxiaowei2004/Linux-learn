#pragma once

#include "task.hpp"
#include "threadpool.hpp"

#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/epoll.h>
#include <sys/socket.h>

#include <errno.h>
#include <string.h>
#include <strings.h>
#include <unistd.h>

#include <iostream>
#include <string>
#include <unordered_map>

// server

class server {
private:
    void Bind() {
        sockaddr_in addr;
        bzero(&addr, sizeof(addr));
        addr.sin_family = AF_INET;
        addr.sin_port = htons(port_);
        addr.sin_addr.s_addr = INADDR_ANY;
        int ret = bind(sockfd_, (sockaddr*)&addr, sizeof(addr));
        if (ret < 0) {
            std::cout << "socket bind fail: " << strerror(errno) << std::endl;
        }
    }

    void Listen() {
        int logback = 5;  // 5 ~ 10
        int ret = listen(sockfd_, logback);
        if (ret < 0) {
            std::cout << "socket listen fail: " << strerror(errno) << std::endl;
        }
    }

    int Accept() {
        sockaddr_in addr;
        bzero(&addr, sizeof(addr));
        socklen_t socklen = sizeof(addr);
        int client_fd = accept(sockfd_, (sockaddr*)&addr, &socklen);
        char ip[32];
        uint16_t port = ntohs(addr.sin_port);
        inet_ntop(AF_INET, &addr.sin_addr, ip, 32);
        printf("accept a new connection.\n");
        printf("clent info: ip = %s, port = %d\n", ip, port);

        struct client_info info;
        info.fd = client_fd;
        info.ip = ip;
        info.port = port;
        clients_[client_fd] = info;

        std::string welcome_msg = "【system】New user [" + std::to_string(client_fd) + "] joined the chatroom!";
        SendAllMsg(welcome_msg, client_fd);

        return client_fd;
    }

    void Acceptor() {
        int client_fd = Accept();
        if (client_fd < 0) {
            printf("connection accept fail.\n");
        }
        struct epoll_event ev;
        ev.events = EPOLLIN;
        ev.data.fd = client_fd;
        epoll_ctl(epfd_, EPOLL_CTL_ADD, client_fd, &ev);
    }

    void SendAllMsg(std::string resp, int exclude_fd = -1) {
        printf("send msg(s) to all client.\n");

        for (auto& client : clients_) {
            int fd = client.second.fd;
            if (fd != exclude_fd)
                send(fd, resp.c_str(), resp.size(), 0);
        }
    }

    void Recver(int sock) {
        char buffer[1024];
        ssize_t n = recv(sock, buffer, sizeof(buffer) - 1, 0);
        if (n <= 0) {
            printf("client quit the connection.\n");
            // 先 epoll 移除后 close 关闭
            if (clients_.find(sock) != clients_.end()) {
                std::string leave_msg = "【system】User [" + std::to_string(sock) + "] leaved the chatroom!";
                SendAllMsg(leave_msg, sock);
                clients_.erase(sock);
            }

            epoll_ctl(epfd_, EPOLL_CTL_DEL, sock, nullptr);
            close(sock);
        } else {
            buffer[n] = 0;
            std::string resp = buffer;
            if (clients_.find(sock) != clients_.end()) {
                std::string formatted_msg = "User[" + std::to_string(sock) + "]: " + buffer;
                SendAllMsg(formatted_msg, sock);
            }
        }
    }

    void handlerEvent(int n) {
        for (int i = 0; i < n; i++) {
            uint32_t event = events_[i].events;
            int sock = events_[i].data.fd;
            if (sock == sockfd_ && (event & EPOLLIN)) {
                // listen 套接字就绪
                Acceptor();
            } else if (event & EPOLLIN) {
                // 普通套接字就绪
                Recver(sock);
            } else if (event & (EPOLLERR | EPOLLHUP)) {
                // 错误或挂起
                printf("socket %d error or hang up\n", sock);
                auto it = clients_.find(sock);
                if (it != clients_.end()) {
                    clients_.erase(it);
                }
                epoll_ctl(epfd_, EPOLL_CTL_DEL, sock, nullptr);
                close(sock);
            }
        }
    }

public:
    server() : port_(8080), ip_("127.0.0.1"), sockfd_(-1), epfd_(-1), events_(nullptr) {
        sockfd_ = socket(AF_INET, SOCK_STREAM, 0);
        if (sockfd_ < 0) {
            std::cout << "socket create fail: " << strerror(errno) << std::endl;
        }

        int opt = 1;
        setsockopt(sockfd_, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));
    }

    void Run() {
        Bind();
        Listen();
        epfd_ = epoll_create(256);
        if (epfd_ < 0) {
            std::cout << "epoll create fail: " << strerror(errno) << std::endl;
        }

        // 先添加 sockfd_
        struct epoll_event ev;
        ev.events = EPOLLIN;
        ev.data.fd = sockfd_;  // 当事件就绪时，可以知道是哪一个 fd
        epoll_ctl(epfd_, EPOLL_CTL_ADD, sockfd_, &ev);
        events_ = new struct epoll_event[10];

        printf("Server Running...\n");

        while (true) {
            int n = epoll_wait(epfd_, events_, 10, -1);
            if (n == 0) {
                printf("timeout.\n");
            } else if (n < 0) {
                if (errno == EINTR)
                    continue;  // 被信号中断，继续
                printf("error.\n");
                break;
            } else {
                handlerEvent(n);
            }
        }
    }

    ~server() {
        if (sockfd_ > 0) {
            close(sockfd_);
            sockfd_ = -1;
        }
        if (epfd_ > 0) {
            close(epfd_);
            epfd_ = -1;
        }

        // 关闭所有客户端连接
        for (auto& client : clients_) {
            close(client.first);
        }
        clients_.clear();

        delete[] events_;
    }

private:
    struct client_info {
        int fd;
        std::string ip;
        uint16_t port;
    };

    std::unordered_map<int, client_info> clients_;
    int epfd_;
    struct epoll_event* events_;

    std::string ip_;
    int sockfd_;
    uint16_t port_;
};

/**
 * now problems:
 * 1. (void)int --> void* args : (void*)(long)client_fd --> int client_fd = (long)args;
 * 2. task t(client_fd, clientThread); clientThread --> 《static》
 * 读 - 输入 / 写 - 输出
 * 3. epoll
 */