#pragma once

#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/socket.h>

#include <errno.h>
#include <fcntl.h>
#include <string.h>
#include <strings.h>
#include <unistd.h>

#include <iostream>
#include <string>

class client {
private:
    int Connect() {
        sockaddr_in addr;
        bzero(&addr, sizeof(addr));
        addr.sin_family = AF_INET;
        addr.sin_port = htons(server_port_);
        inet_pton(AF_INET, server_ip_.c_str(), &addr.sin_addr);
        int ret = connect(sockfd_, (sockaddr*)&addr, sizeof(addr));
        return ret;
    }

    static void* RecvThread(void* args) {
        int sockfd = (long)args;
        char buffer[1024];
        int n = 0;
        while (true) {
            n = read(sockfd, buffer, sizeof(buffer) - 1);
            if (n > 0) {
                buffer[n] = 0;
                printf("%s\n", buffer);
            }
        }
        return nullptr;
    }

public:
    client() : server_port_(8080), server_ip_("127.0.0.1"), sockfd_(-1) {
        sockfd_ = socket(AF_INET, SOCK_STREAM, 0);
        if (sockfd_ < 0) {
            std::cout << "socket create fail: " << strerror(errno) << std::endl;
        }
    }

    void Run() {
        printf("Client running...\n");
        int ret = Connect();
        if (ret < 0) {
            std::cout << "socket connect fail: " << strerror(errno) << std::endl;
        } else {
            printf("********** Welcome come to the online chatroom **********\n");
            printf("You can send messages and receive others\n");
            printf("Enter 'quit' exit chat room\n");
            printf("*********************************************************\n\n");

            std::string msg;
            pthread_t pid;
            pthread_create(&pid, nullptr, RecvThread, (void*)(long)sockfd_);
            pthread_detach(pid);

            while (true) {
                printf("[Me]: ");

                getline(std::cin, msg);
                if (msg == "quit") {
                    break;
                }
                ssize_t n = send(sockfd_, msg.c_str(), msg.size(), 0);
            }
        }
    }

    ~client() {
        if (sockfd_ > 0) {
            close(sockfd_);
            sockfd_ = -1;
        }
    }

private:
    int sockfd_;
    uint16_t server_port_;
    std::string server_ip_;
};