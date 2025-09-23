#pragma once

#include <vector>

#include "log.hpp"
#include "socket.hpp"

using namespace std;

namespace Server {

const static int fdnum = sizeof(fd_set) * 8;
const static int defaultfd = -1;

class selectServer {
private:
    // for debug:
    void print() {
        cout << "fd list: ";
        for (auto& i : fdarray) {
            if (i != defaultfd) {
                cout << i << " ";
            }
        }
        cout << endl;
    }

    void Accepter() {
        logMessage(NORMAL, "Accepter in...");
        string clientip;
        uint16_t clientport;
        int sock = Sock::Accept(listensock, &clientip, &clientport);
        if (sock < 0) {
            logMessage(ERROR, "accept fail");
            exit(ACCEPT_ERR);
        }
        logMessage(NORMAL, "have a new link [%s : %d], sock: %d", clientip.c_str(), clientport, sock);
        // 新的文件描述符放入数组
        int i = 0;
        for (; i < fdarray.size(); i++) {
            if (fdarray[i] != defaultfd) {
                continue;
            }
            break;
        }
        if (i == fdnum) {
            logMessage(WARNING, "server have fulled, please wait...");
        } else {
            fdarray[i] = sock;
        }
        print();
        logMessage(NORMAL, "Accepter off...");
    }

    void Recver(int fd, int pos) {
        logMessage(NORMAL, "Recver in...");
        char buffer[1024];
        ssize_t n = recv(fd, buffer, sizeof(buffer) - 1, 0);
        if (n <= 0) {
            close(fd);
            fdarray[pos] = defaultfd;
            logMessage(WARNING, "client quit");
            return;
        }

        buffer[n] = 0;
        string msg = "client# ";
        msg += buffer;
        write(fd, msg.c_str(), msg.size());
        logMessage(NORMAL, "Recver off...");
    }

    void handlerEvent(fd_set& rset) {
        if (FD_ISSET(listensock, &rset)) {
            Accepter();
        }
        for (int i = 0; i < fdnum; i++) {
            if (fdarray[i] == defaultfd || fdarray[i] == listensock) {
                continue;
            }
            logMessage(DEBUG, "fdarray[%d] = %d", i, fdarray[i]);

            if (FD_ISSET(fdarray[i], &rset)) {
                Recver(fdarray[i], i);
            }
        }
    }

public:
    selectServer(uint16_t _port) : port(_port), listensock(-1), fdarray(fdnum, defaultfd) {}

    void init() {
        listensock = Sock::Socket();
        Sock::Bind(listensock, port);
        Sock::Listen(listensock);
        fdarray[0] = listensock;
    }

    void start() {
        fd_set rset;

        while (true) {
            FD_ZERO(&rset);
            struct timeval timeout = {3, 0};
            int maxfd = fdarray[0];
            for (auto& fd : fdarray) {
                if (fd == defaultfd) {
                    continue;
                } else {
                    FD_SET(fd, &rset);
                }
                maxfd = max(maxfd, fd);
            }
            logMessage(DEBUG, "max fd = %d", maxfd);
            int n = select(maxfd + 1, &rset, nullptr, nullptr, &timeout);
            switch (n) {
                case 0:
                    logMessage(NORMAL, "timeout...");
                    break;
                case -1:
                    logMessage(ERROR, "select fail...");
                    break;
                default:
                    logMessage(DEBUG, "have %d links", n);
                    handlerEvent(rset);
                    break;
            }
        }
    }

    ~selectServer() {
        close(listensock);
        listensock = -1;
    }

private:
    uint16_t port;
    int listensock;
    vector<int> fdarray;  // 存放所有的合法文件描述符
};

}  // namespace Server
