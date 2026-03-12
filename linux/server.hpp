#pragma once

#include "task.hpp"
#include "threadpool.hpp"

#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/socket.h>

#include <errno.h>
#include <string.h>
#include <strings.h>
#include <unistd.h>

#include <iostream>
#include <string>

// server
// multi-pthread version
// pthread pool
// IO: select.poll/epoll

class server {
private:
    // 1. 多线程版：
    // static void* clientThread(void* args) {
    //     char buffer[1024];
    //     int client_fd = (long)args;
    //     while (true) {
    //         ssize_t n = read(client_fd, buffer, sizeof(buffer) - 1);
    //         if (n == 0) {
    //             std::cout << "client exits the process." << std::endl;
    //             close(client_fd);
    //             break;
    //         }
    //         buffer[n] = 0;
    //         std::cout << "receive the msg: " << buffer << ", from " << client_fd << ", and replaying..." <<
    //         std::endl;

    //         std::string response = buffer;
    //         response += "[done]";
    //         // 写
    //         write(client_fd, response.c_str(), response.size());
    //     }

    //     return nullptr;
    // }

    // 2. 线程池版：
    void static clientThread(int client_fd) {
        char buffer[1024];
        while (true) {
            ssize_t n = read(client_fd, buffer, sizeof(buffer) - 1);
            if (n == 0) {
                std::cout << "client exits the process." << std::endl;
                close(client_fd);
                break;
            }
            buffer[n] = 0;
            std::cout << "receive the msg: " << buffer << ", from " << client_fd << ", and replaying..." << std::endl;

            std::string response = buffer;
            response += "[done]";
            write(client_fd, response.c_str(), response.size());
        }
    }

public:
    server() : port_(8080), ip_("127.0.0.1"), sockfd_(-1) {
        sockfd_ = socket(AF_INET, SOCK_STREAM, 0);
        if (sockfd_ < 0) {
            std::cout << "socket create fail: " << strerror(errno) << std::endl;
        }

        int opt = 1;
        setsockopt(sockfd_, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));
    }

    void Bind() {
        sockaddr_in addr;
        bzero(&addr, sizeof(addr));
        addr.sin_family = AF_INET;
        addr.sin_port = htons(port_);
        inet_pton(AF_INET, ip_.c_str(), &addr.sin_addr);
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
        inet_ntop(AF_INET, &addr.sin_addr, ip, 32);
        printf("accept a new connection.\n");
        printf("clent info: ip = %s, port = %d\n", ip, ntohs(addr.sin_port));
        return client_fd;
    }

    void Run() {
        Bind();
        Listen();
        printf("Server Running...\n");
        pool_.threadPoolInit();
        while (true) {
            int client_fd = Accept();
            pthread_t pt;
            // 1. 多线程版：
            // pthread_create(&pt, nullptr, clientThread, (void*)(long)client_fd);
            // pthread_detach(pt);  // 等待退出

            // 2. 线程池版：
            task t(client_fd, clientThread);
            pool_.Push(t);
        }
    }

    ~server() {
        if (sockfd_ > 0) {
            close(sockfd_);
        }
    }

private:
    std::string ip_;
    int sockfd_;
    uint16_t port_;
    threadpool<task> pool_;
};

/**
 * now problems:
 * 1. (void)int --> void* args : (void*)(long)client_fd --> int client_fd = (long)args;
 * 2. task t(client_fd, clientThread); clientThread --> 《static》
 * 读 - 输入 / 写 - 输出
 */
