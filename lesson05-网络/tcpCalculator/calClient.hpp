#pragma once

#include <arpa/inet.h>
#include <pthread.h>
#include <strings.h>
#include <sys/socket.h>

#include "log.hpp"
#include "protocol.hpp"

#include <errno.h>
#include <unistd.h>
#include <cstdlib>
#include <cstring>
#include <iostream>

/*
todo:
- 对内：protobuf
- 对外：json
- 动规
*/

namespace Client {

using namespace std;

enum { USAGE_ERR = 1, SOCKET_ERR, CONNECT_ERR };

class calClient {
private:
    Request parseLine(const string& msg) {
        // "1+1"
        char ops[5] = {'+', '-', '*', '/', '%'};
        int pos = 0;
        int op_pos = 0;
        // O(N)
        for (pos = 0; pos < msg.size(); pos++) {
            if (msg[pos] == '+' || msg[pos] == '-' || msg[pos] == '*' || msg[pos] == '/' || msg[pos] == '%') {
                break;
            }
        }
        int num1 = stoi(msg.substr(0, pos));
        int num2 = stoi(msg.substr(pos + 1, msg.size() - pos));
        return Request(num1, num2, msg[pos]);
    }

public:
    calClient(const string& _serverIp, const uint16_t& _serverPort)
        : serverIp(_serverIp), serverPort(_serverPort), fd(-1) {}

    void initClient() {
        // 1. 创建套接字
        fd = socket(AF_INET, SOCK_STREAM, 0);
        if (fd == -1) {
            logMessage(FATAL, "socket create fail");
            exit(SOCKET_ERR);
        }
    }

    void run() {
        // 2. 发起链接
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
            string inbuffer;
            while (true) {
                cout << "Calculator>>> ";
                getline(cin, message);

                Request req = parseLine(message);
                // Request req(10, 10, '*');
                Response resp;

                string req_str, resp_str, resp_text;
                req.serialize(&req_str);
                string send_str = addLength(req_str);
                send(fd, send_str.c_str(), send_str.size(), 0);

                if (!recvRequset(fd, inbuffer, &resp_str))
                    continue;
                if (!delLength(resp_str, &resp_text))
                    continue;
                resp.deserialize(resp_text);
                cout << "result>>> [" << resp.exitcode << "]" << resp.result << endl;
            }
        }
    }

    ~calClient() {
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
