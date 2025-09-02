#pragma once

#include <arpa/inet.h>
#include <pthread.h>
#include <strings.h>
#include <sys/socket.h>

#include "log.hpp"

#include <errno.h>
#include <unistd.h>
#include <cstdlib>
#include <cstring>
#include <iostream>

namespace Client {

using namespace std;

enum { USAGE_ERR = 1, SOCKET_ERR, CONNECT_ERR };

class tcpClient {
private:
public:
    tcpClient(const string& _serverIp, const uint16_t& _serverPort)
        : serverIp(_serverIp), serverPort(_serverPort), fd(-1) {}

    void initClient() {
        // 1. 创建套接字
        fd = socket(AF_INET, SOCK_STREAM, 0);
        if (fd == -1) {
            logMessage(FATAL, "socket create fail");
            exit(SOCKET_ERR);
        }
        // 2. 不需要显示地绑定
        // 3. 不需要 listen
        // 4. 不需要 accept
    }

    void run() {
        // 5. 需要发起链接
        struct sockaddr_in server;
        server.sin_family = AF_INET;
        server.sin_addr.s_addr = inet_addr(serverIp.c_str());
        server.sin_port = htons(serverPort);

        int n = connect(fd, (sockaddr*)&server, sizeof(server));
        if (n != 0) {
            logMessage(FATAL, "client connect error");
            exit(CONNECT_ERR);
        } else {
            string message;
            while (true) {
                // 进行通信
                cout << "Input# ";
                getline(cin, message);
                write(fd, message.c_str(), message.size());

                char buffer[1024];
                int n = read(fd, buffer, sizeof(buffer) - 1);
                if (n > 0) {
                    buffer[n] = 0;
                    cout << "Server回显" << buffer << endl;
                } else
                    break;
            }
        }
    }

    ~tcpClient() {
        if (fd != -1) {
            close(fd);
            fd = -1;
        }
    }

private:
    string serverIp;
    uint16_t serverPort;
    int fd;
};
}  // namespace Client
