#pragma once

#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/socket.h>

#include <errno.h>
#include <string.h>
#include <strings.h>
#include <unistd.h>

#include <iostream>
#include <string>

// client

class client {
public:
    client() : server_port_(8080), server_ip_("127.0.0.1"), sockfd_(-1) {
        sockfd_ = socket(AF_INET, SOCK_STREAM, 0);
        if (sockfd_ < 0) {
            std::cout << "socket create fail: " << strerror(errno) << std::endl;
        }
    }

    int Connect() {
        sockaddr_in addr;
        bzero(&addr, sizeof(addr));
        addr.sin_family = AF_INET;
        addr.sin_port = htons(server_port_);
        inet_pton(AF_INET, server_ip_.c_str(), &addr.sin_addr);
        int ret = connect(sockfd_, (sockaddr*)&addr, sizeof(addr));
        return ret;
    }

    void Run() {
        printf("Client running...\n");
        int ret = Connect();
        if (ret < 0) {
            std::cout << "socket connect fail: " << strerror(errno) << std::endl;
        } else {
            printf("Connected: communicating with server.\n");
            std::string msg;
            while (true) {
                printf("[client input]$ ");
                getline(std::cin, msg);
                ssize_t n = send(sockfd_, msg.c_str(), msg.size(), 0);

                char buffer[1024];
                n = read(sockfd_, buffer, sizeof(buffer) - 1);
                if (n > 0) {
                    buffer[n] = 0;
                    std::cout << "[replay] " << buffer << std::endl;
                } else
                    break;
            }
        }
    }

    ~client() {
        if (sockfd_ > 0) {
            close(sockfd_);
        }
    }

private:
    int sockfd_;
    uint16_t server_port_;
    std::string server_ip_;
};